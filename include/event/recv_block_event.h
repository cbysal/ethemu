#pragma once

#include "event/event.h"
#include "event/send_block_event.h"

class RecvBlockEvent : public Event {
public:
  Address from;
  Address to;
  Block *block;

  RecvBlockEvent(uint64_t timestamp, Address from, Address to, Block *block);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
               const std::unordered_map<Address, Node *, AddrHash> &nodeMap);
  std::string toString() const;
};
