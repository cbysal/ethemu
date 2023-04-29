#pragma once

#include "core/types/transaction.h"
#include "node/node.h"

class TxEvent {
public:
  const uint64_t timestamp;
  const Id from;
  const Id to;
  const Tx tx;

  TxEvent(uint64_t timestamp, Id from, Id to, Tx tx);

  void process(const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
