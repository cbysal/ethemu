#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

using Id = uint16_t;
using Tx = uint64_t;

const int accountSlots = 16;
const int globalSlots = 5120;
const int accountQueue = 64;
const int globalQueue = 1024;

class TxPool {
private:
  int pendingSize = 0;
  int queuedSize = 0;

  std::unordered_map<Id, std::unordered_map<uint16_t, Tx>> pending;
  std::unordered_map<Id, std::unordered_map<uint16_t, Tx>> queued;

  std::unordered_map<Id, uint16_t> noncer;

  void reorg() {
    for (auto &[id, queuedTxs] : queued) {
      while (queuedTxs.count(noncer[id])) {
        if (pendingSize >= globalSlots && pending[id].size() >= accountSlots)
          break;
        pending[id].emplace(noncer[id], queuedTxs[noncer[id]]);
        pendingSize++;
        queued[id].erase(noncer[id]);
        queuedSize--;
        noncer[id]++;
      }
    }
  }

public:
  void addTx(Tx tx) {
    Id from = (tx >> 16) & 0xffff;
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      reorg();
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      return;
    uint16_t nonce = tx & 0xffff;
    if (queued[from].count(nonce))
      return;
    queued[from][nonce] = tx;
    queuedSize++;
  }

  void notifyBlockTxs(const std::vector<Tx> &txs) {
    std::unordered_map<Id, uint16_t> maxNonces;
    for (Tx tx : txs) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      maxNonces[id] = std::max(maxNonces[id], nonce);
    }
    for (const auto &[id, newNonce] : maxNonces) {
      auto &pendingTxs = pending[id];
      auto &queuedTxs = queued[id];
      uint16_t oldNonce = noncer[id];
      for (int i = oldNonce - 1; i >= 0; i--) {
        if (pendingTxs.count(i)) {
          pendingTxs.erase(i);
          pendingSize--;
          continue;
        }
        break;
      }
      for (int i = oldNonce; i <= newNonce; i++) {
        if (queuedTxs.count(i)) {
          queuedTxs.erase(i);
          queuedSize--;
        }
      }
      noncer[id] = newNonce + 1;
    }
  }

  Tx get(Hash hash) {
    Id from = hash >> 16;
    uint16_t nonce = hash;
    if (pending.count(from) && pending[from].count(nonce))
      return pending[from][nonce];
    if (queued.count(from) && queued[from].count(nonce))
      return queued[from][nonce];
    return 0;
  }

  bool contains(Hash hash) {
    Id from = hash >> 16;
    uint16_t nonce = hash;
    if (pending.count(from) && pending[from].count(nonce))
      return true;
    if (queued.count(from) && queued[from].count(nonce))
      return true;
    return false;
  }

  std::vector<Tx> pollTxs() {
    reorg();
    std::vector<Tx> txs;
    while (txs.size() < 200 && pendingSize > 0) {
      for (auto &[_, pendingTxs] : pending) {
        if (pendingTxs.empty())
          continue;
        auto [nonce, tx] = *pendingTxs.begin();
        txs.push_back(tx);
        pendingTxs.erase(nonce);
        pendingSize--;
      }
    }
    return txs;
  }
};