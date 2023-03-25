#pragma once

#include <unordered_set>

#include "common/types.h"

const int maxKnownTxs = 32768;
const int maxKnownBlocks = 1024;

class KnownCache {
private:
  int max;
  std::unordered_set<Hash, HashHash> hashes;

public:
  KnownCache(int max) { this->max = max; }

  void add(Hash hash) {
    while (hashes.size() > std::max<size_t>(0, max - hashes.size()))
      hashes.erase(hashes.begin());
    hashes.insert(hash);
  }

  bool contains(Hash hash) { return hashes.count(hash); }

  size_t size() { return hashes.size(); }
};

class Peer {
private:
  KnownCache *knownBlocks;
  KnownCache *knownTxs;

public:
  uint64_t addr;

  Peer(uint64_t addr) {
    this->addr = addr;
    this->knownBlocks = new KnownCache(maxKnownBlocks);
    this->knownTxs = new KnownCache(maxKnownTxs);
  }

  ~Peer() {
    delete knownBlocks;
    delete knownTxs;
  }

  bool knownBlock(Hash hash) { return knownBlocks->contains(hash); }
  bool knownTransaction(Hash hash) { return knownTxs->contains(hash); }
  void markBlock(Hash hash) { knownBlocks->add(hash); }
  void markTransaction(Hash hash) { knownTxs->add(hash); }
};