#pragma once

#include <string>
#include <vector>

#include "core/types/transaction.h"

void preGenBlocks(uint64_t simTime, uint64_t minBlockInterval, uint64_t maxBlockInterval, int nodeNum);

class Block {
public:
  uint32_t number;
  Id coinbase;
  std::vector<Tx> txs;

private:
  Block(){};

  Hash txsHash(const std::vector<Tx> &txs) const;

public:
  Block(const uint32_t number, Id coinbase);

  void setTxs(const std::vector<Tx> &);
  Hash hash() const;
};

extern std::vector<std::pair<uint64_t, Block *>> blocks;
