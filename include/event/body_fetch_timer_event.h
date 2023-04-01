#pragma once

#include "event/event.h"

extern const uint64_t bodyFetchInterval;

class BodyFetchTimerEvent : public Event {
public:
  Id id;

  BodyFetchTimerEvent(uint64_t timestamp, Id id);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
