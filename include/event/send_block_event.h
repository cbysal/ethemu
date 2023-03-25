#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_block_event.h"

class SendBlockEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  Block *block;

  SendBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, Block *block);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<uint64_t> &nodeList,
               const std::unordered_map<uint64_t, Node *> &nodeMap);
  std::string toString() const;
};
