#pragma once

#include "event/event.h"

class TxEvent : public Event {
public:
  const uint16_t from;
  const uint16_t to;
  const Tx tx;

  TxEvent(uint64_t timestamp, uint16_t from, uint16_t to, Tx tx);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
