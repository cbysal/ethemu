#include <fstream>
#include <random>

#include "common/math.h"
#include "core/types/block.h"
#include "emu/config.h"

std::vector<std::pair<uint64_t, Block *>> blocks;

void genBlocks(uint64_t simTime, int nodeNum) {
  std::random_device rd;
  std::default_random_engine dre(rd());
  blocks.emplace_back(0, new Block(0, 0, 0));
  uint64_t curTime = global.blockTime;
  int blockNum = 1;
  do {
    Id node = dre() % nodeNum;
    uint16_t size = global.minBlockSize + dre() % (global.maxBlockSize - global.minBlockSize);
    blocks.emplace_back(curTime, new Block(blockNum++, node, size));
    curTime += global.blockTime;
  } while (curTime <= simTime);
}

Block::Block(const uint32_t number, Id coinbase, uint16_t size) : number(number), coinbase(coinbase), size(size) {}

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
