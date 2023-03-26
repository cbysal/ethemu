#pragma once

#include "event/event.h"
#include "event/send_tx_event.h"

class RecvTxEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  Transaction *tx;

  RecvTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, Transaction *tx);

  void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
               const std::vector<Node *> &nodes);
  std::string toString() const;
};
