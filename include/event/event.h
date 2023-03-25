#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "common/types.h"
#include "node/node.h"

class Event {
public:
  uint64_t timestamp;

  Event(uint64_t timestamp) { this->timestamp = timestamp; }

  bool operator<(Event *event) const { return timestamp > event->timestamp; }

  virtual void process(std::priority_queue<Event *> &queue, leveldb::DB *db, const std::vector<Address> &nodeList,
                       const std::unordered_map<Address, Node *, AddrHash> &nodeMpa) {}
  virtual std::string toString() const { return ""; }
};
