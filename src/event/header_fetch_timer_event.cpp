#include "event/header_fetch_timer_event.h"
#include "emu/config.h"
#include "event/header_req_event.h"

const uint64_t headerFetchInterval = 500;

HeaderFetchTimerEvent::HeaderFetchTimerEvent(uint64_t timestamp, uint64_t id) : Event(timestamp), id(id) {}

void HeaderFetchTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                                    const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[id];
  if (timestamp < node->nextFetchHeaderTime) {
    queue.push(new HeaderFetchTimerEvent(node->nextFetchHeaderTime, id));
    return;
  }
  for (auto &[blockHash, owners] : node->fetchingHeaders) {
    uint64_t to = owners[rand() % owners.size()];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new HeaderReqEvent(timestamp + interval, id, to, blockHash));
  }
  node->nextFetchHeaderTime += headerFetchInterval;
  node->fetchingHeaders.clear();
  queue.push(new HeaderFetchTimerEvent(timestamp + headerFetchInterval, id));
}

std::string HeaderFetchTimerEvent::toString() const {
  return "HeaderFetchTimerEvent (timestamp: " + std::to_string(timestamp) + ", id: " + std::to_string(id) + ")";
}
