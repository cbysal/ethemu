#pragma once

#include "event/event.h"

class BodyFetchTimerEvent : public Event {
public:
  uint64_t id;

  BodyFetchTimerEvent(uint64_t timestamp, uint64_t id);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
