#include "core/types/block.h"

std::vector<std::pair<uint64_t, Block *>> blocks;

void preGenBlocks(uint64_t simTime, uint64_t minBlockInterval, uint64_t maxBlockInterval, int nodeNum) {
  uint64_t curTime = 0;
  int blockNum = 0;
  do {
    int node = rand() % nodeNum;
    blocks.emplace_back(curTime, new Block(blockNum++, node));
    uint64_t interval = minBlockInterval + rand() % (maxBlockInterval - minBlockInterval);
    curTime += interval;
  } while (curTime <= simTime);
}

Hash Block::txsHash(const std::vector<Tx> &txs) const {
  Hash hash = 0;
  int off = 0;
  for (Tx tx : txs) {
    Hash txHash = hashTx(tx);
    hash ^= (txHash << off) | (txHash >> (32 - off));
    off = (off + 1) % 32;
  }
  return hash;
}

Block::Block(const uint32_t number, Id coinbase) : number(number), coinbase(coinbase) {}

void Block::setTxs(const std::vector<Tx> &txs) { this->txs = txs; }

Hash Block::hash() const {
  Hash hash = number << 16;
  Hash hash1 = txsHash(txs);
  hash |= (hash1 >> 16) ^ (hash1 & 0xffff);
  return hash;
}