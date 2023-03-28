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
  std::vector<std::shared_ptr<Transaction>> txs;

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
  Block(const std::shared_ptr<Header> &header, const std::vector<std::shared_ptr<Transaction>> &txs)
      : header(header), txs(txs) {}

  Block(uint64_t parentHash, uint64_t coinbase, uint64_t number, const std::vector<std::shared_ptr<Transaction>> &txs)
      : header(std::make_shared<Header>(parentHash, coinbase, number, txsHash(txs))), txs(txs) {}

  uint64_t number() const { return header->number; }

  uint64_t hash() const { return header->hash(); }
};