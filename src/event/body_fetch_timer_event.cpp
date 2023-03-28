#include "event/body_fetch_timer_event.h"
#include "emu/config.h"
#include "event/body_req_event.h"

const uint64_t bodyFetchInterval = 100;

BodyFetchTimerEvent::BodyFetchTimerEvent(uint64_t timestamp, uint16_t id) : Event(timestamp), id(id) {}

void BodyFetchTimerEvent::process(std::priority_queue<Event *, std::vector<Event *>, CompareEvent> &queue,
                                  const std::vector<std::unique_ptr<Node>> &nodes) const {
  const std::unique_ptr<Node> &node = nodes[id];
  if (timestamp < node->nextFetchBodyTime) {
    queue.push(new BodyFetchTimerEvent(node->nextFetchBodyTime, id));
    return;
  }
  for (auto &[blockHash, owners] : node->fetchingBodies) {
    uint16_t to = owners[rand() % owners.size()];
    uint64_t interval = global.minDelay + rand() % (global.maxDelay - global.minDelay);
    queue.push(new BodyReqEvent(timestamp + interval, id, to, blockHash));
  }
  node->fetchingBodies.clear();
  queue.push(new BodyFetchTimerEvent(timestamp + bodyFetchInterval, id));
}

std::string BodyFetchTimerEvent::toString() const {
  return "BodyFetchTimerEvent (timestamp: " + std::to_string(timestamp) + ", id: " + std::to_string(id) + ")";
}
