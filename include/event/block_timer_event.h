#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_block_event.h"

class BlockTimerEvent : public Event {
public:
  BlockTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<uint64_t> &nodeList, const std::unordered_map<uint64_t, Node *> &nodeMap);
  std::string toString() const;
};
