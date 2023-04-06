#include <algorithm>
#include <cstdlib>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/types/transaction.h"

std::vector<std::pair<uint64_t, Tx>> txs;

void preGenTxs(std::vector<std::pair<uint64_t, Block *>> &blocks, uint64_t minTx, uint64_t maxTx, uint64_t prefill,
               int nodeNum) {
  std::vector<int> nonces;
  nonces.resize(nodeNum, 0);
  uint64_t txId = 0;
  for (int i = 1; i < blocks.size(); i++) {
    int txNum = minTx + rand() % (maxTx - minTx + 1);
    uint64_t beginTime = blocks[i - 1].first + 1;
    uint64_t endTime = blocks[i].first;
    std::unordered_set<uint64_t> txTimeSet;
    while (txTimeSet.size() < txNum) {
      uint64_t t = beginTime + rand() % (endTime - beginTime);
      txTimeSet.insert(t);
    }
    if (i == 1) {
      while (txTimeSet.size() < txNum + prefill) {
        uint64_t t = beginTime + rand() % (endTime - beginTime);
        txTimeSet.insert(t);
      }
    }
    std::vector<uint64_t> txTimes;
    for (uint64_t txTime : txTimeSet)
      txTimes.push_back(txTime);
    std::sort(txTimes.begin(), txTimes.end());
    for (uint64_t t : txTimes) {
      int node = rand() % nodeNum;
      txs.emplace_back(t, ((txId++) << 32) | (uint64_t(node) << 16) | nonces[node]++);
    }
  }
}

Hash hashTx(Tx tx) { return tx & 0xffffffff; }
