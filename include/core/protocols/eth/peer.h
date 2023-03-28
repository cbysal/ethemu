#pragma once

#include <unordered_set>

const int maxKnownTxs = 32768;
const int maxKnownBlocks = 1024;

class KnownCache {
private:
  int max;
  std::unordered_set<uint64_t> hashes;

public:
  KnownCache(int max) { this->max = max; }

  void add(uint64_t hash) {
    while (hashes.size() > std::max<size_t>(0, max - hashes.size()))
      hashes.erase(hashes.begin());
    hashes.insert(hash);
  }

  bool contains(uint64_t hash) { return hashes.count(hash); }

  size_t size() { return hashes.size(); }
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

  bool knownBlock(uint64_t hash) { return knownBlocks->contains(hash); }
  bool knownTransaction(uint64_t hash) { return knownTxs->contains(hash); }
  void markBlock(uint64_t hash) { knownBlocks->add(hash); }
  void markTransaction(uint64_t hash) { knownTxs->add(hash); }
};