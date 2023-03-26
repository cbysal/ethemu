#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_block_event.h"

class BlockTimerEvent : public Event {
public:
  BlockTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<Node *> &nodes);
  std::string toString() const;
};
