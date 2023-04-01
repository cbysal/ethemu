#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/protocols/eth/peer.h"
#include "core/txpool/txpool.h"
#include "core/types/block.h"
#include "core/types/transaction.h"

struct Node {
  Id id;
  uint16_t nonce;
  uint64_t current;
  TxPool *txPool;
  std::unordered_map<Hash, std::shared_ptr<Block>> blocksByNumber;
  std::unordered_map<Hash, std::shared_ptr<Block>> blocksByHash;
  std::vector<Id> peerList;
  std::unordered_map<Id, Peer *> peerMap;

  uint64_t nextFetchHeaderTime;
  uint64_t nextFetchBodyTime;

  std::unordered_map<Hash, std::vector<Id>> fetchingHeaders;
  std::unordered_map<Hash, std::shared_ptr<Header>> fetchedHeaders;
  std::unordered_map<Hash, std::vector<Id>> fetchingBodies;

  std::unordered_map<uint32_t, uint64_t> minTxTimestamp;

  Node(Id id) {
    this->id = id;
    this->nonce = 0;
    this->current = 0;
    this->nextFetchHeaderTime = 0;
    this->nextFetchBodyTime = 0;
  }

  ~Node() {
    for (auto &[_, peer] : peerMap)
      delete peer;
    delete txPool;
  }

  void setTxNum(int txNum) { txPool = new TxPool(txNum); }
  void addPeer(Node *node) {
    peerList.push_back(node->id);
    peerMap[node->id] = new Peer(node->id);
  }
  void insertBlock(const std::shared_ptr<Block> &block) {
    blocksByNumber[block->number()] = block;
    blocksByHash[block->hash()] = block;
    current = block->number();
    txPool->notifyBlockTxs(block->txs);
    for (Tx tx : block->txs)
      minTxTimestamp.erase(tx >> 32);
  }
  std::vector<Tx> getTxs() { return txPool->pollTxs(); }
};
