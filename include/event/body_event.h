#pragma once

#include "event/event.h"

class BodyEvent : public Event {
public:
  const Id from;
  const Id to;
  const Hash blockHash;
  const std::vector<Tx> txs;

  BodyEvent(uint64_t timestamp, Id from, Id to, Hash blockHash, const std::vector<Tx> &txs);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
