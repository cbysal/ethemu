#pragma once

#include <string>
#include <vector>

#include "common/math.h"
#include "common/types.h"
#include "core/types/transaction.h"

struct Block {
  Hash parentHash;
  uint64_t coinbase;
  uint64_t number;
  std::vector<Transaction *> transactions;

  Block(){};

  Block(Hash parentHash, uint64_t coinbase, uint64_t number, const std::vector<Transaction *> &transactions) {
    this->parentHash = parentHash;
    this->coinbase = coinbase;
    this->number = number;
    this->transactions = transactions;
  }

  static Block *parse(const std::string &data) {
    Block *block = new Block();
    block->parentHash.parse(data.substr(0, 32));
    block->coinbase = u64FromString(data.substr(32, 8));
    block->number = u64FromString(data.substr(40, 8));
    int txNum = (data.length() - 48) / sizeof(Transaction);
    for (int i = 0; i < txNum; i++) {
      Transaction *tx = new Transaction();
      tx->parse(data.substr(48 + i * sizeof(Transaction), sizeof(Transaction)));
      block->transactions.push_back(tx);
    }
    return block;
  }

  std::string bytes() const {
    std::string data;
    data += parentHash.bytes();
    data += u64ToString(coinbase);
    data += u64ToString(number);
    for (auto tx : transactions)
      data += tx->bytes();
    return data;
  }

  Hash hash() const {
    std::string raw = bytes();
    Hash h;
    int off = 0;
    for (char b : raw) {
      h.data[off] ^= b;
      off = (off + 1) % 32;
    }
    return h;
  }
};