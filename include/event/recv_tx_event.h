#pragma once

#include "event/event.h"
#include "event/send_tx_event.h"

class RecvTxEvent : public Event {
public:
  Address from;
  Address to;
  Transaction *tx;

  RecvTxEvent(uint64_t timestamp, Address from, Address to, Transaction *tx);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
               const std::unordered_map<Address, Node *, AddrHash> &nodeMap);
  std::string toString() const;
};
