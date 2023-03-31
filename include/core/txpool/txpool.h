#pragma once

#include <cstdint>
#include <queue>
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

  std::unordered_map<Id, std::priority_queue<Tx, std::vector<Tx>, std::greater<>>> pending;
  std::unordered_map<Id, std::priority_queue<Tx, std::vector<Tx>, std::greater<>>> queued;

  uint32_t minTx = 0;
  std::unordered_set<uint32_t> allTxs;

  std::unordered_map<Id, uint16_t> noncer;

  void reorg() {
    for (auto &[id, queuedTxs] : queued) {
      while (!queuedTxs.empty() && (queuedTxs.top() & 0xffff) == noncer[id]) {
        if (pendingSize >= globalSlots && pending[id].size() >= accountSlots)
          break;
        pending[id].push(queuedTxs.top());
        pendingSize++;
        queuedTxs.pop();
        queuedSize--;
        noncer[id]++;
      }
    }
  }

public:
  void addTx(Tx tx) {
    Id from = tx >> 16;
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      reorg();
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      return;
    if ((tx >> 32) >= minTx)
      allTxs.insert(tx >> 32);
    while (allTxs.count(minTx))
      allTxs.erase(minTx++);
    queued[from].push(tx);
    queuedSize++;
  }

  void notifyBlockTxs(const std::vector<Tx> &txs) {
    std::unordered_map<Id, uint16_t> maxNonces;
    for (Tx tx : txs) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      maxNonces[id] = std::max(maxNonces[id], nonce);
      if ((tx >> 32) >= minTx)
        allTxs.insert(tx >> 32);
    }
    while (allTxs.count(minTx))
      allTxs.erase(minTx++);
    for (const auto &[id, newNonce] : maxNonces) {
      auto &pendingTxs = pending[id];
      auto &queuedTxs = queued[id];
      while (!pendingTxs.empty() && (pendingTxs.top() & 0xffff) <= newNonce) {
        pendingTxs.pop();
        pendingSize--;
      }
      while (!queuedTxs.empty() && (queuedTxs.top() & 0xffff) <= newNonce) {
        queuedTxs.pop();
        queuedSize--;
      }
      noncer[id] = newNonce + 1;
    }
  }

  bool contains(uint32_t txId) {
    if (txId < minTx)
      return true;
    return allTxs.count(txId);
  }

  std::vector<Tx> pollTxs() {
    reorg();
    std::vector<Tx> txs;
    while (txs.size() < 200 && pendingSize > 0) {
      for (auto &[_, pendingTxs] : pending) {
        if (pendingTxs.empty())
          continue;
        Tx tx = pendingTxs.top();
        pendingTxs.pop();
        txs.push_back(tx);
        pendingSize--;
      }
    }
    return txs;
  }
};