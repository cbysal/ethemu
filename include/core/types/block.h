#pragma once

#include <cstring>
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
      Transaction *tx = Transaction::parse(data.substr(16 + i * sizeof(Transaction), sizeof(Transaction)));
      block->transactions.push_back(tx);
    }
    return block;
  }

  std::string bytes() const {
    std::string data;
    data.resize(24 + transactions.size() * sizeof(Transaction));
    char *dataPtr = data.data();
    *((uint64_t *)dataPtr) = parentHash;
    *((uint64_t *)(dataPtr + 8)) = coinbase;
    *((uint64_t *)(dataPtr + 16)) = number;
    for (int i = 0; i < transactions.size(); i++) {
      std::string txBytes = transactions[i]->bytes();
      char *txBytesPtr = txBytes.data();
      std::memcpy(dataPtr + 16 + i * sizeof(Transaction), txBytesPtr, sizeof(Transaction));
    }
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