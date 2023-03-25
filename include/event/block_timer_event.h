#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_block_event.h"

class BlockTimerEvent : public Event {
public:
  BlockTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
               const std::unordered_map<Address, Node *, AddrHash> &nodeMap);
  std::string toString() const;
};
