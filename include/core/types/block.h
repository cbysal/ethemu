#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "common/math.h"
#include "core/types/header.h"
#include "core/types/transaction.h"

class Block {
public:
  std::shared_ptr<Header> header;
  std::vector<std::shared_ptr<Transaction>> transactions;

private:
  Block(){};

  uint64_t txsHash(const std::vector<std::shared_ptr<Transaction>> &txs) const {
    uint64_t hash = 0;
    for (int i = 0; i < txs.size(); i++) {
      uint64_t txHash = txs[i]->hash();
      hash ^= (txHash << i) | (txHash >> (64 - i));
    }
    return hash;
  }

public:
  Block(uint64_t parentHash, uint64_t coinbase, uint64_t number,
        const std::vector<std::shared_ptr<Transaction>> &transactions)
      : header(std::make_shared<Header>(parentHash, coinbase, number, txsHash(transactions))),
        transactions(transactions) {}

  static std::unique_ptr<Block> parse(const std::string &data) {
    Block *block = new Block();
    block->header = Header::parse(data.substr(0, 24));
    int txNum = (data.length() - 24) / sizeof(Transaction);
    for (int i = 0; i < txNum; i++) {
      std::unique_ptr<Transaction> tx =
          Transaction::parse(data.substr(24 + i * sizeof(Transaction), sizeof(Transaction)));
      block->transactions.push_back(std::move(tx));
    }
    return std::unique_ptr<Block>(block);
  }

  uint64_t number() const { return header->number; }

  std::string bytes() const {
    std::string data;
    data.resize(32 + transactions.size() * sizeof(Transaction));
    char *dataPtr = data.data();
    std::string headerBytes = header->bytes();
    char *headerBytesPtr = headerBytes.data();
    std::memcpy(dataPtr, headerBytesPtr, 32);
    for (int i = 0; i < transactions.size(); i++) {
      std::string txBytes = transactions[i]->bytes();
      char *txBytesPtr = txBytes.data();
      std::memcpy(dataPtr + 16 + i * sizeof(Transaction), txBytesPtr, sizeof(Transaction));
    }
    return data;
  }

  uint64_t hash() const { return header->hash(); }
};