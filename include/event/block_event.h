#pragma once

#include "core/types/block.h"
#include "node/node.h"

class BlockEvent {
public:
  const uint64_t timestamp;
  const Id from;
  const Id to;
  Block *block;

  BlockEvent(uint64_t timestamp, Id from, Id to, Block *block);

  void process(const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
