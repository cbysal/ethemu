#pragma once

#include <string>

#include "common/math.h"

struct Transaction {
  uint64_t from;
  uint64_t to;
  uint64_t nonce;

  Transaction() {}

  Transaction(uint64_t from, uint64_t to, uint64_t nonce) {
    this->from = from;
    this->to = to;
    this->nonce = nonce;
  }

  uint64_t hash() const { return (from << 48) | (to << 32) | nonce; }

  void parse(const std::string &data) {
    from = u64FromBytes(data.substr(0, 8));
    to = u64FromBytes(data.substr(8, 8));
    nonce = u64FromBytes(data.substr(16, 8));
  }

  std::string bytes() const {
    std::string data;
    data += u64ToBytes(from);
    data += u64ToBytes(to);
    data += u64ToBytes(nonce);
    return data;
  }
};