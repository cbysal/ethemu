#pragma once

#include "event/event.h"

class BodyReqEvent : public Event {
public:
  const Id from;
  const Id to;
  const Hash blockHash;

  BodyReqEvent(uint64_t timestamp, Id from, Id to, Hash blockHash);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<Node *> &nodes) const;
  std::string toString() const;
};
