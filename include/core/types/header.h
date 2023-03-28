#pragma once

#include <cstdint>

using Id = uint16_t;
using Hash = uint32_t;

class Header {
public:
  Hash parentHash;
  Id coinbase;
  uint32_t number;
  Hash txsRoot;

private:
  Header() {}

public:
  Header(Hash parentHash, Id coinbase, uint32_t number, Hash txsRoot)
      : parentHash(parentHash), coinbase(coinbase), number(number), txsRoot(txsRoot) {}

  Hash hash() const {
    return (((parentHash ^ (parentHash << 8) ^ (parentHash << 16) ^ (parentHash << 24))) & 0xff000000) |
           (((coinbase << 8) & (coinbase << 16)) & 0x00ff0000) |
           (((number >> 16) ^ (number >> 8) ^ number ^ (number << 8)) & 0x0000ff0000) |
           (((txsRoot >> 24) ^ (txsRoot >> 16) ^ (txsRoot >> 8) ^ 8) & 0x000000ff);
  }
};