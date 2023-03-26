#pragma once

#include <string>
#include <vector>

#include "common/math.h"
#include "core/types/transaction.h"

class Block {
public:
  uint64_t parentHash;
  uint64_t coinbase;
  uint64_t number;
  std::vector<Transaction *> transactions;

private:
  Block(){};

public:
  Block(uint64_t parentHash, uint64_t coinbase, uint64_t number, const std::vector<Transaction *> &transactions) {
    this->parentHash = parentHash;
    this->coinbase = coinbase;
    this->number = number;
    this->transactions = transactions;
  }

  static Block *parse(const std::string &data) {
    Block *block = new Block();
    block->parentHash = u64FromBytes(data.substr(0, 8));
    block->coinbase = u64FromBytes(data.substr(8, 8));
    block->number = u64FromBytes(data.substr(16, 8));
    int txNum = (data.length() - 16) / sizeof(Transaction);
    for (int i = 0; i < txNum; i++) {
      Transaction *tx = new Transaction();
      tx->parse(data.substr(16 + i * sizeof(Transaction), sizeof(Transaction)));
      block->transactions.push_back(tx);
    }
    return block;
  }

  std::string bytes() const {
    std::string data;
    data += u64ToBytes(parentHash);
    data += u64ToBytes(coinbase);
    data += u64ToBytes(number);
    for (auto tx : transactions)
      data += tx->bytes();
    return data;
  }

  uint64_t hash() const {
    uint64_t h = 0;
    h ^= parentHash ^ (parentHash << 32);
    for (auto tx : transactions) {
      uint64_t txHash = tx->hash();
      h ^= txHash ^ (txHash << 32);
    }
    h &= 0xffffffff00000000;
    h |= (coinbase << 16) | number;
    return h;
  }
};