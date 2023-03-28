#pragma once

#include "event/event.h"

class TxReqEvent : public Event {
public:
  const uint16_t from;
  const uint16_t to;
  const uint64_t txHash;

  TxReqEvent(uint64_t timestamp, uint16_t from, uint16_t to, uint64_t txHash);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
