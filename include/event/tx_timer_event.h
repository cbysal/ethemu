#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_tx_event.h"

class TxTimerEvent : public Event {
public:
  TxTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<Node *> &nodes);
  std::string toString() const;
};
