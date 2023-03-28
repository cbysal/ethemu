#pragma once

#include "event/event.h"

class BodyFetchTimerEvent : public Event {
public:
  Id id;

  BodyFetchTimerEvent(uint64_t timestamp, Id id);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
