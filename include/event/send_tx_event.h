#pragma once

#include "event/event.h"
#include "event/recv_tx_event.h"

class SendTxEvent : public Event {
public:
  uint64_t from;
  uint64_t to;
  Transaction *tx;

  SendTxEvent(uint64_t timestamp, uint64_t from, uint64_t to, Transaction *tx);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<uint64_t> &nodeList,
               const std::unordered_map<uint64_t, Node *> &nodeMap);
  std::string toString() const;
};
