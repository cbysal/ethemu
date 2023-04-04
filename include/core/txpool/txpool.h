#pragma once

#include <bitset>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

#include "bitset.h"

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

  BitSet allTxs;

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
  TxPool(int txNum) { allTxs.resize(txNum); }

  bool addTx(Tx tx) {
    Id from = tx >> 16;
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      reorg();
    if (queuedSize >= globalQueue || queued[from].size() >= accountQueue)
      return false;
    allTxs.set(tx >> 32);
    queued[from].push(tx);
    queuedSize++;
    return true;
  }

  void notifyBlockTxs(const std::vector<Tx> &txs) {
    std::unordered_map<Id, uint16_t> maxNonces;
    for (Tx tx : txs) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      maxNonces[id] = std::max(maxNonces[id], nonce);
      allTxs.set(tx >> 32);
    }
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
      noncer[id] = std::max<Id>(noncer[id], newNonce + 1);
    }
  }

  bool contains(uint32_t txId) { return allTxs[txId]; }

  std::vector<Tx> pollTxs() {
    reorg();
    std::vector<Tx> txs;
    while (txs.size() < 200 && pendingSize > 0) {
      for (auto &[_, pendingTxs] : pending) {
        if (txs.size() >= 200)
          break;
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

  int size() const { return pendingSize + queuedSize; }
};