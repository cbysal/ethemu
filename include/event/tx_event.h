#pragma once

#include "event/event.h"

class TxEvent : public Event {
public:
  const Id from;
  const Id to;
  const Tx tx;

  TxEvent(uint64_t timestamp, Id from, Id to, Tx tx);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
