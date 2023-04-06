#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/protocols/eth/peer.h"
#include "core/txpool/txpool.h"
#include "core/types/block.h"
#include "core/types/transaction.h"
#include "event/event.h"

struct Node {
  Id id;
  uint64_t current;
  TxPool *txPool;
  std::vector<Id> peerList;
  std::unordered_map<Id, Peer *> peerMap;

  std::unordered_map<uint32_t, uint64_t> minTxTimestamp;

  std::queue<std::tuple<Id, Id, bool, Tx>> resentTxs;

  Node(Id id) {
    this->id = id;
    this->current = 0;
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
  void insertBlock(Block *block) {
    current = block->number;
    txPool->notifyBlockTxs(block->txs);
    for (Tx tx : block->txs)
      minTxTimestamp.erase(tx >> 32);
  }
};
