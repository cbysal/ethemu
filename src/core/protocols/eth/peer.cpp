#include "core/protocols/eth/peer.h"

const int maxKnownBlocks = 1024;

KnownCache::KnownCache(int max) { this->max = max; }

void KnownCache::add(Hash hash) {
  while (hashes.size() > max)
    hashes.erase(hashes.begin());
  hashes.insert(hash);
}

bool KnownCache::contains(Hash hash) { return hashes.count(hash); }

Peer::Peer(Id id) {
  this->id = id;
  this->knownBlocks = new KnownCache(maxKnownBlocks);
}

Peer::~Peer() { delete knownBlocks; }

bool Peer::knownBlock(Hash hash) { return knownBlocks->contains(hash); }
void Peer::markBlock(Hash hash) { knownBlocks->add(hash); }
