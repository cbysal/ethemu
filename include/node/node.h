#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core/protocols/eth/peer.h"
#include "core/rawdb/accessors_chain.h"
#include "core/types/block.h"
#include "core/types/transaction.h"
#include "leveldb/db.h"

struct Node {
  std::string id;
  uint64_t addr;
  leveldb::DB *db;
  uint64_t nonce;
  uint64_t current;
  std::unordered_map<uint64_t, Transaction *> txPool;
  std::vector<Peer *> peerList;
  std::unordered_map<uint64_t, Peer *> peerMap;

  Node(std::string id, uint64_t addr, leveldb::DB *db) {
    this->id = id;
    this->addr = addr;
    this->db = db;
    this->nonce = 0;
    this->current = 0;
  }

  uint64_t nextNonce() { return nonce++; }
  void addPeer(Node *node) {
    Peer *peer = new Peer(node->addr);
    peerList.push_back(peer);
    peerMap[node->addr] = peer;
  }
  void insertBlock(Block *block) {
    writeBlock(db, id, block);
    current = block->number;
  }
};
