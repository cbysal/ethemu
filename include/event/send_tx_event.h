#pragma once

#include "event/event.h"
#include "event/recv_tx_event.h"

class SendTxEvent : public Event {
public:
  Address from;
  Address to;
  Transaction *tx;

  SendTxEvent(uint64_t timestamp, Address from, Address to, Transaction *tx);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
               const std::unordered_map<Address, Node *, AddrHash> &nodeMap);
  std::string toString() const;
};
