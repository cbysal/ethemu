#pragma once

#include "event/event.h"

extern const uint64_t headerFetchInterval;

class HeaderFetchTimerEvent : public Event {
public:
  Id id;

  HeaderFetchTimerEvent(uint64_t timestamp, Id id);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
