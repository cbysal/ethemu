#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core/protocols/eth/peer.h"
#include "core/types/block.h"
#include "core/types/transaction.h"

struct Node {
  uint64_t id;
  uint64_t nonce;
  uint64_t current;
  std::unordered_map<uint64_t, std::shared_ptr<Transaction>> txPool;
  std::unordered_map<uint64_t, std::shared_ptr<Block>> blocksByNumber;
  std::unordered_map<uint64_t, std::shared_ptr<Block>> blocksByHash;
  std::vector<uint64_t> peerList;
  std::unordered_map<uint64_t, Peer *> peerMap;

  uint64_t nextFetchHeaderTime;
  uint64_t nextFetchBodyTime;

  std::unordered_map<uint64_t, std::vector<uint64_t>> fetchingHeaders;
  std::unordered_map<uint64_t, std::shared_ptr<Header>> fetchedHeaders;
  std::unordered_map<uint64_t, std::vector<uint64_t>> fetchingBodies;

  Node(uint64_t id) {
    this->id = id;
    this->nonce = 0;
    this->current = 0;
    this->nextFetchHeaderTime = 0;
    this->nextFetchBodyTime = 0;
  }

  ~Node() {
    for (auto &[_, peer] : peerMap)
      delete peer;
  }

  uint64_t nextNonce() { return nonce++; }
  void addPeer(const std::unique_ptr<Node> &node) {
    peerList.push_back(node->id);
    peerMap[node->id] = new Peer(node->id);
  }
  void insertBlock(const std::shared_ptr<Block> &block) {
    blocksByNumber[block->number()] = block;
    blocksByHash[block->hash()] = block;
    current = block->number();
  }
};
