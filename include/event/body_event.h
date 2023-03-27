#pragma once

#include "event/event.h"

class BodyEvent : public Event {
public:
  const uint64_t from;
  const uint64_t to;
  const uint64_t blockHash;
  const std::vector<std::shared_ptr<Transaction>> txs;

  BodyEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t blockHash,
            const std::vector<std::shared_ptr<Transaction>> &txs);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};