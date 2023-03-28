#pragma once

#include <memory>
#include <string>

class Header {
public:
  uint64_t parentHash;
  uint16_t coinbase;
  uint64_t number;
  uint64_t txsRoot;

private:
  Header() {}

public:
  Header(uint64_t parentHash, uint16_t coinbase, uint64_t number, uint64_t txsRoot)
      : parentHash(parentHash), coinbase(coinbase), number(number), txsRoot(txsRoot) {}

  uint64_t hash() const {
    uint64_t h = 0;
    h ^= parentHash ^ (parentHash << 32);
    h ^= txsRoot ^ (txsRoot << 32);
    h &= 0xffffffff00000000;
    h |= (coinbase << 16) | number;
    return h;
  }
};