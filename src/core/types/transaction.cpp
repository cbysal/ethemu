#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/types/transaction.h"

std::vector<std::pair<uint64_t, Tx>> txs;

void outputTxs(const std::string &file) {
  std::ofstream ofs(file.c_str());
  ofs << "timestamp,id,nonce" << std::endl;
  for (auto &[timestamp, tx] : txs) {
    ofs << timestamp << ',' << (tx >> 16 & 0xffff) << ',' << (tx & 0xffff) << std::endl;
  }
  ofs.close();
}

void genTxs(std::vector<std::pair<uint64_t, Block *>> &blocks, uint64_t minTx, uint64_t maxTx, uint64_t prefill,
               int nodeNum) {
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::vector<int> nonces;
  nonces.resize(nodeNum, 0);
  uint64_t txId = 0;
  for (int i = 1; i < blocks.size(); i++) {
    int txNum = minTx + dre() % (maxTx - minTx + 1);
    uint64_t beginTime = blocks[i - 1].first + 1;
    uint64_t endTime = blocks[i].first;
    std::unordered_set<uint64_t> txTimeSet;
    while (txTimeSet.size() < txNum) {
      uint64_t t = beginTime + dre() % (endTime - beginTime);
      txTimeSet.insert(t);
    }
    if (i == 1) {
      while (txTimeSet.size() < txNum + prefill) {
        uint64_t t = beginTime + dre() % (endTime - beginTime);
        txTimeSet.insert(t);
      }
    }
    std::vector<uint64_t> txTimes;
    for (uint64_t txTime : txTimeSet)
      txTimes.push_back(txTime);
    std::sort(txTimes.begin(), txTimes.end());
    for (uint64_t t : txTimes) {
      int node = dre() % nodeNum;
      txs.emplace_back(t, ((txId++) << 32) | (uint64_t(node) << 16) | nonces[node]++);
    }
  }
}

Hash hashTx(Tx tx) { return tx & 0xffffffff; }
