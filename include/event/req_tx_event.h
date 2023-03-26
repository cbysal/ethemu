#pragma once

#include "event/event.h"

class ReqTxEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  uint64_t txHash;

  ReqTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, uint64_t txHash);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};