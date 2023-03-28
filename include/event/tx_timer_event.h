#pragma once

#include "event/event.h"

class TxTimerEvent : public Event {
public:
  TxTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
