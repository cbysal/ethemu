#pragma once

#include <bitset>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Id = uint16_t;
using Tx = uint64_t;

const int accountSlots = 16;
const int globalSlots = 5120;
const int accountQueue = 64;
const int globalQueue = 1024;

class TxPool {
private:
  std::vector<Tx> pending;
  std::vector<Tx> queued;
  std::vector<uint16_t> pendingSizes;
  std::vector<uint16_t> queuedSizes;
  std::vector<uint16_t> noncer;

  void reorg() {
    std::vector<uint16_t> nonces = noncer;
    for (Tx tx : pending) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      nonces[id] = std::max<uint16_t>(nonces[id], nonce + 1);
    }
    std::vector<Tx> newPending;
    std::vector<Tx> newQueued;
    std::sort(queued.begin(), queued.end());
    for (Tx tx : queued) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      if (nonce != nonces[id]) {
        newQueued.push_back(tx);
        continue;
      }
      if (pending.size() >= globalSlots && pendingSizes[id] >= accountSlots) {
        newQueued.push_back(tx);
        continue;
      }
      pending.push_back(tx);
      pendingSizes[id]++;
      queuedSizes[id]--;
      nonces[id]++;
    }
    queued = newQueued;
  }

public:
  TxPool(int nodeNum) {
    pendingSizes.resize(nodeNum);
    queuedSizes.resize(nodeNum);
    noncer.resize(nodeNum, 0);
  }

  bool addTx(Tx tx) {
    Id id = tx >> 16;
    if (queued.size() >= globalQueue || queuedSizes[id] >= accountQueue) {
      reorg();
    }
    if (queued.size() >= globalQueue || queuedSizes[id] >= accountQueue)
      return false;
    queued.push_back(tx);
    queuedSizes[id]++;
    return true;
  }

  void notifyBlockTxs(const std::vector<Tx> &txs) {
    for (Tx tx : txs) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      noncer[id] = std::max<uint16_t>(noncer[id], nonce + 1);
    }
    std::vector<Tx> newPending;
    std::vector<Tx> newQueued;
    for (Tx tx : pending) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      if (nonce >= noncer[id]) {
        newPending.push_back(tx);
      } else {
        pendingSizes[id]--;
      }
    }
    for (Tx tx : queued) {
      Id id = tx >> 16;
      uint16_t nonce = tx;
      if (nonce >= noncer[id]) {
        newQueued.push_back(tx);
      } else {
        queuedSizes[id]--;
      }
    }
    pending = newPending;
    queued = newQueued;
  }

  std::vector<Tx> pollTxs() {
    reorg();
    std::sort(pending.begin(), pending.end());
    int num = 150 + rand() % 11;
    std::vector<Tx> txs;
    std::vector<Tx> newPending;
    for (Tx tx : pending) {
      if (txs.size() >= num) {
        newPending.push_back(tx);
        continue;
      }
      Id id = tx >> 16;
      uint16_t nonce = tx;
      if (nonce == noncer[id]) {
        txs.push_back(tx);
        pendingSizes[id]--;
        noncer[id]++;
      } else {
        newPending.push_back(tx);
      }
    }
    pending = newPending;
    return txs;
  }

  int size() const { return pending.size() + queued.size(); }
};