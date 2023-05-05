#pragma once

#include <string>
#include <vector>

#include "core/types/transaction.h"

void genBlocks(uint64_t simTime, int nodeNum);

class Block {
public:
  uint32_t number;
  Id coinbase;
  uint16_t size;
  std::vector<Tx> txs;

private:
  Block(){};

public:
  Block(const uint32_t number, Id coinbase, uint16_t size);

  void setTxs(const std::vector<Tx> &);
  Hash hash() const;
};

extern std::vector<std::pair<uint64_t, Block *>> blocks;

void outputBlocks(const std::string &file);
