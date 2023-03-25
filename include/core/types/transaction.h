#pragma once

#include <string>

#include "common/math.h"
#include "common/types.h"

struct Transaction {
  Address from;
  Address to;
  uint64_t nonce;

  Transaction() {}

  Transaction(Address from, Address to, uint64_t nonce) {
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
    from.parse(data.substr(0, 32));
    to.parse(data.substr(32, 64));
    nonce = u64FromString(data.substr(64, 72));
  }

  std::string bytes() const {
    std::string data;
    data += from.bytes();
    data += to.bytes();
    data += u64ToString(nonce);
    return data;
  }
};