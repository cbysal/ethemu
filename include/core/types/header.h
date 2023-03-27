#pragma once

#include <memory>
#include <string>

class Header {
public:
  uint64_t parentHash;
  uint64_t coinbase;
  uint64_t number;
  uint64_t txsRoot;

private:
  Header() {}

public:
  Header(uint64_t parentHash, uint64_t coinbase, uint64_t number, uint64_t txsRoot)
      : parentHash(parentHash), coinbase(coinbase), number(number), txsRoot(txsRoot) {}

  static std::shared_ptr<Header> parse(const std::string &data) {
    const char *dataPtr = data.data();
    Header *header = new Header();
    header->parentHash = *((uint64_t *)dataPtr);
    header->coinbase = *((uint64_t *)(dataPtr + 8));
    header->number = *((uint64_t *)(dataPtr + 16));
    header->txsRoot = *((uint64_t *)(dataPtr + 24));
    return std::shared_ptr<Header>(header);
  }

  uint64_t hash() const {
    uint64_t h = 0;
    h ^= parentHash ^ (parentHash << 32);
    h ^= txsRoot ^ (txsRoot << 32);
    h &= 0xffffffff00000000;
    h |= (coinbase << 16) | number;
    return h;
  }

  std::string bytes() const {
    std::string data;
    data.resize(32);
    char *dataPtr = data.data();
    *((uint64_t *)dataPtr) = parentHash;
    *((uint64_t *)(dataPtr + 8)) = coinbase;
    *((uint64_t *)(dataPtr + 16)) = number;
    *((uint64_t *)(dataPtr + 24)) = txsRoot;
    return data;
  }
};