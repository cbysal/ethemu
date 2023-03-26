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
  std::unordered_map<uint64_t, std::shared_ptr<Transaction>> txPool;
  std::vector<uint64_t> peerList;
  std::unordered_map<uint64_t, std::unique_ptr<Peer>> peerMap;

  Node(std::string id, uint64_t addr, leveldb::DB *db) {
    this->id = id;
    this->addr = addr;
    this->db = db;
    this->nonce = 0;
    this->current = 0;
  }

  uint64_t nextNonce() { return nonce++; }
  void addPeer(const std::unique_ptr<Node> &node) {
    std::unique_ptr<Peer> peer = std::make_unique<Peer>(node->addr);
    peerList.push_back(node->addr);
    peerMap[node->addr] = std::move(peer);
  }
  void insertBlock(const std::shared_ptr<Block> &block) {
    writeBlock(db, id, block);
    current = block->number;
  }
};
