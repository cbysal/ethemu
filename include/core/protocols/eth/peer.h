#pragma once

#include <unordered_set>

using Hash = uint32_t;

const int maxKnownTxs = 32768;
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
  KnownCache *knownTxs;

public:
  uint16_t id;

  Peer(uint16_t id) {
    this->id = id;
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