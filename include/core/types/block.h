#pragma once

#include <string>
#include <vector>

#include "core/types/transaction.h"

class Block {
public:
  uint32_t number;
  std::vector<Tx> txs;

private:
  Block(){};

  Hash txsHash(const std::vector<Tx> &txs) const {
    Hash hash = 0;
    int off = 0;
    for (Tx tx : txs) {
      Hash txHash = hashTx(tx);
      hash ^= (txHash << off) | (txHash >> (32 - off));
      off = (off + 1) % 32;
    }
    return hash;
  }

public:
  Block(const uint32_t number, const std::vector<Tx> &txs) : number(number), txs(txs) {}

  Hash hash() const {
    Hash hash = number << 16;
    Hash hash1 = txsHash(txs);
    hash |= (hash1 >> 16) ^ (hash1 & 0xffff);
    return hash;
  }
};