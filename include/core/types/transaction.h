#pragma once

#include <string>

#include "common/math.h"

class Transaction {
public:
  uint16_t from;
  uint16_t to;
  uint16_t nonce;

private:
  Transaction() {}

public:
  Transaction(uint16_t from, uint16_t to, uint16_t nonce) {
    this->from = from;
    this->to = to;
    this->nonce = nonce;
  }

  uint64_t hash() const { return (uint64_t(from) << 48) | (uint64_t(to) << 32) | nonce; }
};