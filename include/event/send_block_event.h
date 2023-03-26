#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_block_event.h"

class SendBlockEvent : public Event {
public:
  const uint64_t from;
  const uint64_t to;
  const std::shared_ptr<Block> block;

  SendBlockEvent(uint64_t timestamp, uint64_t from, uint64_t to, const std::shared_ptr<Block> &block);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
