#include "event/header_fetch_timer_event.h"
#include "common/math.h"
#include "emu/config.h"
#include "event/header_req_event.h"

const uint64_t headerFetchInterval = 500;

HeaderFetchTimerEvent::HeaderFetchTimerEvent(uint64_t timestamp, Id id) : Event(timestamp), id(id) {}

void HeaderFetchTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                                    const std::vector<Node *> &nodes) const {
  Node *node = nodes[id];
  if (timestamp < node->nextFetchHeaderTime) {
    queue.push(new HeaderFetchTimerEvent(node->nextFetchHeaderTime, id));
    return;
  }
  for (auto &[blockHash, owners] : node->fetchingHeaders) {
    Id to = owners[rand() % owners.size()];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new HeaderReqEvent(timestamp + interval, id, to, blockHash));
  }
  node->nextFetchHeaderTime += headerFetchInterval;
  node->fetchingHeaders.clear();
  queue.push(new HeaderFetchTimerEvent(timestamp + headerFetchInterval, id));
}

std::string HeaderFetchTimerEvent::toString() const {
  return "HeaderFetchTimerEvent (timestamp: " + std::to_string(timestamp) + ", id: " + idToString(id) + ")";
}
