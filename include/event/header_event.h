#pragma once

#include "event/event.h"

class HeaderEvent : public Event {
public:
  const Id from;
  const Id to;
  const std::shared_ptr<Header> header;

  HeaderEvent(uint64_t timestamp, Id from, Id to, const std::shared_ptr<Header> &header);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
