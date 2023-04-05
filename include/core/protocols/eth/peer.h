#pragma once

#include <cstdint>
#include <unordered_set>

using Id = uint16_t;
using Hash = uint32_t;

class KnownCache {
private:
  int max;
  std::unordered_set<Hash> hashes;

public:
  KnownCache(int max);

  void add(Hash hash);
  bool contains(Hash hash);
};

class Peer {
private:
  KnownCache *knownBlocks;

public:
  Id id;

  Peer(Id id);
  ~Peer();

  bool knownBlock(Hash hash);
  void markBlock(Hash hash);
};