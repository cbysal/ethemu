#pragma once

#include "event/event.h"

class BlockTimerEvent : public Event {
public:
  BlockTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
