#pragma once

#include "event/event.h"
#include "event/send_block_event.h"

class RecvBlockEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  Block *block;

  RecvBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, Block *block);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<Node *> &nodes);
  std::string toString() const;
};
