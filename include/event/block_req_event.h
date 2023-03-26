#pragma once

#include "event/event.h"

class BlockReqEvent : public Event {
public:
  const uint64_t from;
  const uint64_t to;
  const uint64_t blockHash;

  BlockReqEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t blockHash);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
