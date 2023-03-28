#pragma once

#include <queue>
#include <string>

#include "node/node.h"

class CompareEvent;

class Event {
public:
  const uint64_t timestamp;

  Event(uint64_t timestamp) : timestamp(timestamp) {}

  bool operator<(const Event *&event) const { return timestamp < event->timestamp; }

  virtual void process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                       const std::vector<std::unique_ptr<Node>> &nodes) const {}
  virtual std::string toString() const { return ""; }
};

struct CompareEvent {
  bool operator()(const Event *e1, const Event *e2) const { return e1->timestamp > e2->timestamp; }
};