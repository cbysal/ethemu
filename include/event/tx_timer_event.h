#pragma once

#include "emu/config.h"
#include "event/event.h"
#include "event/recv_tx_event.h"

class TxTimerEvent : public Event {
public:
  TxTimerEvent(uint64_t timestamp);

  void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<uint64_t> &nodeList,
               const std::unordered_map<uint64_t, Node *> &nodeMap);
  std::string toString() const;
};
