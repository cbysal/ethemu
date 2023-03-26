#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/tx_event.h"

class TxTimerEvent : public Event {
public:
  TxTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<std::unique_ptr<Node>> &nodes) const;
  std::string toString() const;
};
