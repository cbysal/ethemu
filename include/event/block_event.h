#pragma once

#include "event/event.h"

class BlockEvent : public Event {
public:
  const Id from;
  const Id to;
  const bool byHash;
  const std::shared_ptr<Block> block;

  BlockEvent(uint64_t timestamp, Id from, Id to, bool byHash, const std::shared_ptr<Block> &block);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
