#pragma once

#include <unordered_set>

using Id = uint16_t;
using Hash = uint32_t;

const int maxKnownBlocks = 1024;

class KnownCache {
private:
  int max;
  std::unordered_set<Hash> hashes;

public:
  KnownCache(int max) { this->max = max; }

  void add(Hash hash) {
    while (hashes.size() > max)
      hashes.erase(hashes.begin());
    hashes.insert(hash);
  }

  bool contains(Hash hash) { return hashes.count(hash); }
};

class Peer {
private:
  KnownCache *knownBlocks;

public:
  Id id;

  Peer(Id id) {
    this->id = id;
    this->knownBlocks = new KnownCache(maxKnownBlocks);
  }

  ~Peer() {
    delete knownBlocks;
  }

  bool knownBlock(Hash hash) { return knownBlocks->contains(hash); }
  void markBlock(Hash hash) { knownBlocks->add(hash); }
};