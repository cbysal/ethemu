#pragma once

#include "event/event.h"

class HeaderFetchTimerEvent : public Event {
public:
  uint16_t id;

  HeaderFetchTimerEvent(uint64_t timestamp, uint16_t id);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
