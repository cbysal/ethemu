#pragma once

#include "event/event.h"

class TxHashEvent : public Event {
public:
  const Id from;
  const Id to;
  const Hash txHash;

  TxHashEvent(uint64_t timestamp, Id from, Id to, uint64_t txHash);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
