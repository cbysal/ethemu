#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/txpool/txpool.h"
#include "core/types/block.h"
#include "core/types/transaction.h"

struct Node {
  Id id;
  uint64_t current;
  TxPool *txPool;
  std::vector<Id> peerList;

  std::queue<std::tuple<Id, Id, Tx>> resentTxs;

  Node(Id id) {
    this->id = id;
    this->current = 0;
  }

  ~Node() { delete txPool; }

  void setTxNum(int txNum) { txPool = new TxPool(txNum); }
  void addPeer(Node *node) { peerList.push_back(node->id); }
  void insertBlock(Block *block) {
    current = block->number;
    txPool->notifyBlockTxs(block->txs);
  }
};
