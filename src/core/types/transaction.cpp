#include <cstdlib>
#include <utility>
#include <vector>

#include "core/types/transaction.h"

std::vector<std::pair<uint64_t, Tx>> txs;

void preGenTxs(uint64_t simTime, uint64_t minTxInterval, uint64_t maxTxInterval, uint64_t prefill, int nodeNum) {
  std::vector<int> nonces;
  nonces.resize(nodeNum, 0);
  uint64_t txId = 0;
  uint64_t curTime = 0;
  uint64_t prefilled = 0;
  do {
    int node = rand() % nodeNum;
    txs.emplace_back(curTime, ((txId++) << 32) | (uint64_t(node) << 16) | nonces[node]++);
    if (prefilled < prefill) {
      int node = rand() % nodeNum;
      txs.emplace_back(curTime, ((txId++) << 32) | (uint64_t(node) << 16) | nonces[node]++);
      prefilled++;
    }
    uint64_t interval = minTxInterval + rand() % (maxTxInterval - minTxInterval);
    curTime += interval;
  } while (curTime < simTime);
}

Hash hashTx(Tx tx) { return tx & 0xffffffff; }
