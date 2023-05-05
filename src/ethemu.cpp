#include <atomic>
#include <csignal>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

#include "BS_thread_pool.hpp"
#include "common/math.h"
#include "core/types/transaction.h"
#include "cxxopts.hpp"
#include "emu/config.h"
#include "ethemu.h"
#include "node/node.h"

using Options = cxxopts::Options;
using Option = cxxopts::Option;

Options options("Ethemu");

Option datadirOpt("datadir", "Data directory for the databases and keystore", cxxopts::value<std::string>());
Option nodesOpt("nodes", "Number of emulated nodes", cxxopts::value<int>()->default_value("100"));
Option minersOption("miners", "Number of emulated miners", cxxopts::value<int>()->default_value("20"));
Option peersOpt("peers", "Average peer num of all nodes", cxxopts::value<int>()->default_value("20"));
Option delayMinOpt("delay.min", "Minimum delay between peers", cxxopts::value<int>()->default_value("40"));
Option delayMaxOpt("delay.max", "Maximum delay between peers", cxxopts::value<int>()->default_value("80"));
Option bwMinOpt("bw.min", "Minimum bandwidth between peers", cxxopts::value<int>()->default_value("2048"));
Option bwMaxOpt("bw.max", "Maximum bandwidth between peers", cxxopts::value<int>()->default_value("4096"));
Option minBlockOpt("block.min", "Minimum size of a block", cxxopts::value<int>()->default_value("60"));
Option maxBlockOpt("block.max", "Maximum size of a block", cxxopts::value<int>()->default_value("120"));
Option txMinOpt("tx.min", "Restrict the minimum transaction num in a block",
                cxxopts::value<int>()->default_value("150"));
Option txMaxOpt("tx.max", "Restrict the maximum transaction num in a block",
                cxxopts::value<int>()->default_value("160"));
Option blockTimeOpt("block.time", "Interval of consensus for blocks", cxxopts::value<int>()->default_value("12000"));
Option prefillOpt("prefill", "Fill transaction pool with transactions before emulation",
                  cxxopts::value<int>()->default_value("0"));
Option simTimeOpt("sim.time", "Time limit of the simulation", cxxopts::value<int>()->default_value("1000000"));
Option verbosityOpt("verbosity", "Show all outputs if it is on", cxxopts::value<bool>());

std::vector<Option> opts = {datadirOpt,   nodesOpt,   minersOption, peersOpt,    delayMinOpt, delayMaxOpt,
                            bwMinOpt,     bwMaxOpt,   minBlockOpt,  maxBlockOpt, txMinOpt,    txMaxOpt,
                            blockTimeOpt, simTimeOpt, prefillOpt,   verbosityOpt};

std::vector<uint64_t> dijkstra(const std::vector<std::vector<std::pair<uint16_t, uint64_t>>> &graph, Id from) {
  std::priority_queue<std::pair<uint64_t, uint16_t>, std::vector<std::pair<uint64_t, uint16_t>>, std::greater<>> pq;
  std::vector<uint64_t> result(graph.size(), UINT64_MAX);
  result[from] = 0;
  pq.emplace(0, from);
  while (!pq.empty()) {
    Id from = pq.top().second;
    pq.pop();
    if (result[from] == UINT64_MAX)
      break;
    for (auto &[id, len] : graph[from]) {
      if (result[id] > result[from] + len) {
        result[id] = result[from] + len;
        pq.emplace(result[id], id);
      }
    }
  }
  return result;
}

typedef uint16_t EventType;

const EventType TxEvent = 0;
const EventType MineEvent = 1;
const EventType BlockEvent = 2;

typedef std::tuple<uint64_t, EventType, Id, Hash> Event;

const int PARALLEL = 16;
std::vector<Node *> nodes;
std::vector<Event> events;
std::atomic_int blockEventId = 1;
std::atomic_int txEventId = 0;
std::mutex eventMu;
std::vector<size_t> rootEvents;
std::unordered_map<size_t, std::vector<size_t>> eventsTree;
BS::thread_pool pool(PARALLEL);

bool eventCmp(const Event &e1, const Event &e2) {
  if (std::get<0>(e1) != std::get<0>(e2))
    return std::get<0>(e1) < std::get<0>(e2);
  return std::get<1>(e1) < std::get<1>(e2);
}

void merge(std::vector<Event> &newEvents) {
  int i1 = events.size() - 1, i2 = newEvents.size() - 1;
  events.resize(events.size() + newEvents.size());
  for (int i = events.size() - 1; i >= 0; i--) {
    if (i1 >= 0 && i2 >= 0) {
      if (eventCmp(events[i1], newEvents[i2])) {
        events[i] = std::move(newEvents[i2--]);
      } else {
        events[i] = std::move(events[i1--]);
      }
    } else if (i1 < 0) {
      std::copy(newEvents.begin(), newEvents.begin() + i2, events.begin());
      break;
    } else {
      break;
    }
  }
}

void processTxEvent(Id id, Hash hash) {
  Tx tx = txs[hash].second;
  uint32_t txId = tx >> 32;
  Node *node = nodes[id];
  if (node->txPool->contains(txId))
    return;
  bool isAdded = node->txPool->addTx(tx);
  if (!isAdded) {
    node->resentTxs.push(tx);
    return;
  }
}

void processMineEvent(uint64_t timestamp, Id id, Hash hash) {
  Block *block = blocks[hash].second;
  Node *node = nodes[id];
  std::vector<Tx> txs = node->txPool->pollTxs();
  block->setTxs(txs);
  std::cout << "New Block (Number: " << block->number << ", Hash: " << hashHex(block->hash())
            << ", Coinbase: " << idToString(node->id) << ", Txs: " << block->txs.size() << ")" << std::endl;
  std::cout << timestamp << " id " << idToString(node->id) << " txpool " << node->txPool->size() << std::endl;
  if (block->number <= node->current)
    return;
  node->insertBlock(block);
  while (!node->resentTxs.empty()) {
    Tx &tx = node->resentTxs.front();
    uint32_t txId = tx >> 32;
    if (node->txPool->contains(txId)) {
      node->resentTxs.pop();
      continue;
    }
    bool isAdded = node->txPool->addTx(tx);
    if (!isAdded) {
      break;
    }
    node->resentTxs.pop();
  }
}

void processBlockEvent(Id id, Hash hash) {
  Block *block = blocks[hash].second;
  Node *node = nodes[id];
  if (block->number <= node->current)
    return;
  node->insertBlock(block);
  while (!node->resentTxs.empty()) {
    Tx &tx = node->resentTxs.front();
    uint32_t txId = tx >> 32;
    if (node->txPool->contains(txId)) {
      node->resentTxs.pop();
      continue;
    }
    bool isAdded = node->txPool->addTx(tx);
    if (!isAdded) {
      break;
    }
    node->resentTxs.pop();
  }
}

void process(int curId) {
  uint64_t timestamp;
  EventType type;
  Id id;
  Hash hash;
  while (true) {
    std::tie(timestamp, type, id, hash) = events[curId];
    switch (type) {
    case TxEvent:
      processTxEvent(id, hash);
      break;
    case MineEvent:
      processMineEvent(timestamp, id, hash);
      break;
    case BlockEvent:
      processBlockEvent(id, hash);
      break;
    }
    if (eventsTree.find(curId) == eventsTree.end()) {
      break;
    }
    curId = eventsTree[curId][0];
  }
}

int main(int argc, char *argv[]) {
  std::for_each(opts.begin(), opts.end(), [](auto opt) { options.add_option("", opt); });
  auto result = options.parse(argc, argv);
  switch (result.unmatched().size()) {
  case 0:
    ethemu(result["datadir"].as<std::string>(), result["sim.time"].as<int>(), result["prefill"].as<int>(),
           result["verbosity"].as<bool>());
    break;
  case 1: {
    auto subcmd = result.unmatched()[0];
    if (subcmd == "gen") {
      gen(result);
      break;
    }
    std::cerr << "Unknown subcommand: " + subcmd << std::endl;
    return -1;
  }
  default:
    std::cerr << "Too many subcommands!" << std::endl;
  }
  return 0;
}

void genBlockEvents() {
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::vector<Event> localEvents;
  std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays(nodes.size());
  int curId;
  while ((curId = blockEventId++) < blocks.size()) {
    auto &[blockTime, block] = blocks[curId];
    for (int i = 0; i < nodes.size(); i++) {
      Node *node = nodes[i];
      std::vector<std::pair<uint16_t, uint64_t>> &delay = delays[i];
      delay.resize(node->peers.size());
      int n = std::sqrt(node->peers.size());
      for (int j = 0; j < n; j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer) + block->size * 1024 / std::get<2>(peer)};
      }
      for (int j = n; j < node->peers.size(); j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer) * 5 + block->size * 1024 / std::get<2>(peer) + dre() % 500};
      }
      delays.push_back(delay);
    }
    std::vector<uint64_t> timeSpent = dijkstra(delays, block->coinbase);
    Id mineId = std::min_element(timeSpent.begin(), timeSpent.end()) - timeSpent.begin();
    for (Id id = 0; id < nodes.size(); id++) {
      if (id == mineId)
        localEvents.emplace_back(blockTime + timeSpent[id], MineEvent, id, block->number);
      else
        localEvents.emplace_back(blockTime + timeSpent[id], BlockEvent, id, block->number);
    }
    std::sort(localEvents.begin(), localEvents.end(), eventCmp);
    if (eventMu.try_lock()) {
      merge(localEvents);
      localEvents.clear();
      eventMu.unlock();
    }
  }
  eventMu.lock();
  merge(localEvents);
  eventMu.unlock();
}

void genTxEvents() {
  std::vector<Event> localEvents;
  std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays(nodes.size());
  int curId;
  while ((curId = txEventId++) < txs.size()) {
    auto &[txTime, tx] = txs[curId];
    for (Id i = 0; i < nodes.size(); i++) {
      Node *node = nodes[i];
      std::vector<std::pair<uint16_t, uint64_t>> &delay = delays[i];
      delay.resize(node->peers.size());
      int n = std::sqrt(delay.size());
      for (int j = 0; j < n; j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer)};
      }
      for (int j = n; j < delay.size(); j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer) * 3};
      }
    }
    std::vector<uint64_t> timeSpent = dijkstra(delays, (Id)(tx >> 16));
    for (Id id = 0; id < nodes.size(); id++) {
      localEvents.emplace_back(txTime + timeSpent[id], TxEvent, id, tx >> 32);
    }
    if (eventMu.try_lock()) {
      merge(localEvents);
      localEvents.clear();
      eventMu.unlock();
    }
  }
  eventMu.lock();
  merge(localEvents);
  eventMu.unlock();
}

void genEventTree() {
  std::vector<size_t> blockEventId(blocks.size());
  std::vector<size_t> lastEventId(nodes.size());
  for (size_t i = 0; i < events.size(); i++) {
    uint64_t timestamp;
    EventType type;
    Id id;
    Hash hash;
    std::tie(timestamp, type, id, hash) = events[i];
    switch (type) {
    case TxEvent:
      eventsTree[lastEventId[id]].push_back(i);
      break;
    case MineEvent:
      rootEvents.push_back(i);
      blockEventId[hash] = i;
      break;
    case BlockEvent:
      eventsTree[blockEventId[hash]].push_back(i);
      break;
    }
    lastEventId[id] = i;
  }
}

void ethemu(const std::string &dataDir, uint64_t simTime, uint64_t prefill, bool verbosity) {
  loadConfig(dataDir);
  for (int i = 0; i < global.nodes.size(); i++) {
    const EmuNode &emuNode = global.nodes[i];
    Node *node = new Node(emuNode.id);
    nodes.push_back(node);
  }
  for (auto &node : nodes) {
    for (const auto &peer : global.nodes[node->id].peers) {
      node->addPeer(peer);
    }
  }
  events.reserve(blocks.size() * nodes.size() + txs.size() * nodes.size());
  genBlocks(simTime, nodes.size());
  genTxs(blocks, global.minTx, global.maxTx, prefill, nodes.size());
  for (Id i = 0; i < nodes.size(); i++) {
    if (i == 0) {
      events.emplace_back(0, MineEvent, i, 0);
    } else {
      events.emplace_back(0, BlockEvent, i, 0);
    }
  }
  for (int i = 0; i < PARALLEL; i++) {
    pool.submit([] {
      genBlockEvents();
      genTxEvents();
    });
  }
  pool.wait_for_tasks();
  for (auto &node : nodes)
    node->setTxNum(txs.size());
  genEventTree();
  for (size_t rootEvent : rootEvents) {
    process(rootEvent);
    for (int nextEvent : eventsTree[rootEvent]) {
      pool.submit([](int nextEvent) { process(nextEvent); }, nextEvent);
    }
    pool.wait_for_tasks();
  }
  outputTxs("txs.csv");
  outputBlocks("blocks.csv");
  for (Node *node : nodes)
    delete node;
  std::cerr << "Mem: ";
  pid_t pid = getpid();
  system(std::string("cat /proc/" + std::to_string(pid) + "/status | grep VmHWM | awk '{print $2 $3}' > /dev/fd/2")
             .data());
}
