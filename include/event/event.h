#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "common/types.h"
#include "node/node.h"

class CompareEvent;

class Event {
public:
  uint64_t timestamp;

  Event(uint64_t timestamp) { this->timestamp = timestamp; }

  bool operator<(const Event *&event) const { return timestamp < event->timestamp; }

  virtual void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue, leveldb::DB *db,
                       const std::vector<uint64_t> &nodeList, const std::unordered_map<uint64_t, Node *> &nodeMpa) {}
  virtual std::string toString() const { return ""; }
};

struct CompareEvent {
  bool operator()(const Event *e1, const Event *e2) const { return e1->timestamp > e2->timestamp; }
};