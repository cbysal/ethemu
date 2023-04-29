#include <fstream>

#include "common/math.h"
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

Block::Block(const uint32_t number, Id coinbase) : number(number), coinbase(coinbase) {}

void Block::setTxs(const std::vector<Tx> &txs) { this->txs = txs; }

Hash Block::hash() const { return number; }

void outputBlocks(const std::string &file) {
  std::ofstream ofs(file);
  ofs << "timestamp,number,coinbase,txs" << std::endl;
  for (auto &[timestamp, block] : blocks) {
    ofs << timestamp << ',' << block->number << ',' << block->coinbase << ',';
    std::string txs;
    for (Tx tx : block->txs) {
      txs += hashHex(tx) + ' ';
    }
    if (!txs.empty())
      txs.pop_back();
    ofs << txs << std::endl;
  }
  ofs.close();
}
