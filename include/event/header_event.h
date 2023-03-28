#pragma once

#include "core/types/header.h"
#include "event/event.h"

class HeaderEvent : public Event {
public:
  const uint16_t from;
  const uint16_t to;
  const std::shared_ptr<Header> header;

  HeaderEvent(uint64_t timestamp, uint16_t from, uint16_t to, const std::shared_ptr<Header> &header);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
