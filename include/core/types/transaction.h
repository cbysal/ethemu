#pragma once

#include <string>

#include "common/math.h"

class Transaction {
public:
  uint64_t from;
  uint64_t to;
  uint64_t nonce;

private:
  Transaction() {}

public:
  Transaction(uint64_t from, uint64_t to, uint64_t nonce) {
    this->from = from;
    this->to = to;
    this->nonce = nonce;
  }

  uint64_t hash() const { return (from << 48) | (to << 32) | nonce; }

  static Transaction *parse(const std::string &data) {
    Transaction *tx = new Transaction();
    const char *dataPtr = data.data();
    tx->from = *((uint64_t *)dataPtr);
    tx->to = *((uint64_t *)(dataPtr + 8));
    tx->nonce = *((uint64_t *)(dataPtr + 16));
    return tx;
  }

  std::string bytes() const {
    std::string data;
    data.resize(24);
    char *dataPtr = data.data();
    *((uint64_t *)dataPtr) = from;
    *((uint64_t *)(dataPtr + 8)) = to;
    *((uint64_t *)(dataPtr + 16)) = nonce;
    return data;
  }
};