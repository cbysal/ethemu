#pragma once

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
  std::vector<std::tuple<Id, uint16_t, uint16_t>> peers;

  std::queue<Tx> resentTxs;

  Node(Id id) {
    this->id = id;
    this->current = 0;
  }

  ~Node() { delete txPool; }

  void setTxPool(int nodeNum) { txPool = new TxPool(nodeNum); }
  void addPeer(const std::tuple<Id, uint16_t, uint16_t> &peer) { peers.push_back(peer); }
  void insertBlock(Block *block) {
    current = block->number;
    txPool->notifyBlockTxs(block->txs);
  }
};
