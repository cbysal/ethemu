#pragma once

#include <string>

#include "common/math.h"
#include "common/types.h"

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

  void parse(const std::string &data) {
    from = u64FromString(data.substr(0, 8));
    to = u64FromString(data.substr(8, 8));
    nonce = u64FromString(data.substr(16, 8));
  }

  std::string bytes() const {
    std::string data;
    data += u64ToString(from);
    data += u64ToString(to);
    data += u64ToString(nonce);
    return data;
  }
};