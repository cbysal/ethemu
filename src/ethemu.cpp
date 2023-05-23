#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <csignal>
#include <fstream>
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

const uint16_t MACHINE_WAIT_TX = 16;
const uint16_t MACHINE_WAIT_BLOCK = 5;
const uint32_t MACHINE_PROC = 500000;

typedef uint16_t EventType;

const EventType TxEvent = 0;
const EventType MineEvent = 1;
const EventType BlockEvent = 2;

typedef std::tuple<uint64_t, EventType, Id, Hash> Event;

const int PARALLEL = 16;
bool verbosity = false;
std::vector<Node *> nodes;
std::atomic_int curTxId = 0;
std::vector<Event> events;
std::mutex eventMu;
std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> txLog;
std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> blockLog;
std::mutex txLogMu;
BS::thread_pool pool(PARALLEL);

void processTxEvent(Id id, Hash hash) {
  Tx tx = txs[hash].second;
  Node *node = nodes[id];
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
  std::cout << timestamp << " New Block (Number: " << block->number << ", Hash: " << hashHex(block->hash())
            << ", Coinbase: " << idToString(node->id) << ", Txs: " << block->txs.size() << ") TxPool "
            << node->txPool->size() << std::endl;
  if (block->number <= node->current)
    return;
  node->insertBlock(block);
  while (!node->resentTxs.empty()) {
    Tx &tx = node->resentTxs.front();
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
    bool isAdded = node->txPool->addTx(tx);
    if (!isAdded) {
      break;
    }
    node->resentTxs.pop();
  }
}

void process(const Event &event) {
  uint64_t timestamp;
  EventType type;
  Id id;
  Hash hash;
  std::tie(timestamp, type, id, hash) = event;
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
}

void process(int curId) {
  const Event &event = events[curId];
  process(event);
}

void process(const std::vector<Event> &events) {
  for (const Event &event : events) {
    process(event);
  }
}

int main(int argc, char *argv[]) {
  std::for_each(opts.begin(), opts.end(), [](auto opt) { options.add_option("", opt); });
  auto result = options.parse(argc, argv);
  switch (result.unmatched().size()) {
  case 0:
    verbosity = result["verbosity"].as<bool>();
    ethemu(result["datadir"].as<std::string>(), result["sim.time"].as<int>(), result["prefill"].as<int>());
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

uint64_t genBlockEvents(Hash blockId, std::default_random_engine &dre) {
  std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays(nodes.size());
  auto &[blockTime, block] = blocks[blockId];
  for (int i = 0; i < nodes.size(); i++) {
    Node *node = nodes[i];
    std::vector<std::pair<uint16_t, uint64_t>> &delay = delays[i];
    delay.resize(node->peers.size());
    int left = std::sqrt(node->peers.size());
    for (int j = 0; j < node->peers.size(); j++) {
      const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
      if (dre() % (node->peers.size() - j) < left) {
        delay[j] = {std::get<0>(peer), std::get<1>(peer) + uint64_t(block->size) * 1024 / std::get<2>(peer) +
                                           MACHINE_WAIT_BLOCK + uint64_t(block->size) * 1024 / MACHINE_PROC};
        left--;
      } else {
        delay[j] = {std::get<0>(peer), std::get<1>(peer) * 5 + uint64_t(block->size) * 1024 / std::get<2>(peer) +
                                           dre() % 500 + MACHINE_WAIT_BLOCK +
                                           uint64_t(block->size) * 1024 / MACHINE_PROC};
      }
    }
  }
  std::vector<uint64_t> timeSpent = dijkstra(delays, block->coinbase);
  if (verbosity) {
    std::vector<uint64_t> times = timeSpent;
    std::sort(times.begin(), times.end());
    blockLog.emplace_back(times[times.size() / 2], times[times.size() * 9 / 10], times.back());
  }
  uint64_t maxTimestamp = blockTime + *std::max_element(timeSpent.begin(), timeSpent.end());
  for (Id id = 0; id < nodes.size(); id++) {
    if (id == block->coinbase)
      events.emplace_back(blockTime + timeSpent[id], MineEvent, id, block->number);
    else
      events.emplace_back(blockTime + timeSpent[id], BlockEvent, id, block->number);
  }
  return maxTimestamp;
}

void genTxEvents(Hash end) {
  std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays(nodes.size());
  int curId;
  while ((curId = curTxId++) < end) {
    auto &[txTime, tx] = txs[curId];
    for (Id i = 0; i < nodes.size(); i++) {
      Node *node = nodes[i];
      std::vector<std::pair<uint16_t, uint64_t>> &delay = delays[i];
      delay.resize(node->peers.size());
      int n = std::sqrt(delay.size());
      for (int j = 0; j < n; j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer) + MACHINE_WAIT_TX};
      }
      for (int j = n; j < delay.size(); j++) {
        const std::tuple<Id, uint16_t, uint16_t> &peer = node->peers[j];
        delay[j] = {std::get<0>(peer), std::get<1>(peer) * 3 + MACHINE_WAIT_TX};
      }
    }
    std::vector<uint64_t> timeSpent = dijkstra(delays, (Id)(tx >> 16));
    if (verbosity) {
      std::vector<uint64_t> times = timeSpent;
      std::sort(times.begin(), times.end());
      txLogMu.lock();
      txLog.emplace_back(times[times.size() / 2], times[times.size() * 9 / 10], times.back());
    }
    txLogMu.unlock();
    eventMu.lock();
    for (Id id = 0; id < nodes.size(); id++) {
      events.emplace_back(txTime + timeSpent[id], TxEvent, id, tx >> 32);
    }
    eventMu.unlock();
  }
  curTxId--;
}

std::pair<Event, std::vector<std::vector<Event>>> genEventTree() {
  uint8_t counter[nodes.size()];
  std::memset(counter, 0, nodes.size() * sizeof(uint8_t));
  Event mineEvent;
  std::vector<std::vector<Event>> eventLists(nodes.size());
  std::vector<Event> newEvents;
  Id mineId = UINT16_MAX;
  for (const Event &event : events) {
    uint64_t timestamp;
    EventType type;
    Id id;
    Hash hash;
    std::tie(timestamp, type, id, hash) = event;
    switch (type) {
    case TxEvent:
      if (counter[id] < 2) {
        eventLists[id].push_back(event);
      } else {
        newEvents.push_back(event);
      }
      break;
    case MineEvent:
    case BlockEvent:
      if (counter[id] < 1) {
        eventLists[id].push_back(event);
      } else {
        newEvents.push_back(event);
      }
      if (type == MineEvent && mineId == UINT16_MAX) {
        mineId = id;
      }
      counter[id]++;
      break;
    }
  }
  mineEvent = eventLists[mineId].front();
  eventLists[mineId].erase(eventLists[mineId].begin());
  events = newEvents;
  return {mineEvent, eventLists};
}

void outputTxLog(const std::string &file) {
  std::ofstream ofs(file);
  ofs << "50%,90%,100%" << std::endl;
  for (auto &[p50, p90, p100] : txLog) {
    ofs << p50 << ',' << p90 << ',' << p100 << std::endl;
  }
  ofs.close();
}

void outputBlockLog(const std::string &file) {
  std::ofstream ofs(file);
  ofs << "50%,90%,100%" << std::endl;
  for (auto &[p50, p90, p100] : blockLog) {
    ofs << p50 << ',' << p90 << ',' << p100 << std::endl;
  }
  ofs.close();
}

void ethemu(const std::string &dataDir, uint64_t simTime, uint64_t prefill) {
  loadConfig(dataDir);
  for (int i = 0; i < global.nodes.size(); i++) {
    const EmuNode &emuNode = global.nodes[i];
    Node *node = new Node(emuNode.id);
    nodes.push_back(node);
  }
  for (auto &node : nodes) {
    node->setTxPool(nodes.size());
    for (const auto &peer : global.nodes[node->id].peers) {
      node->addPeer(peer);
    }
  }
  genBlocks(simTime, nodes.size());
  genTxs(blocks, global.minTx, global.maxTx, prefill, nodes.size());
  for (Id i = 0; i < nodes.size(); i++) {
    if (i == 0) {
      events.emplace_back(0, MineEvent, i, 0);
    } else {
      events.emplace_back(0, BlockEvent, i, 0);
    }
  }
  std::random_device rd;
  std::default_random_engine dre(rd());
  for (Hash curBlock = 1; curBlock < blocks.size(); curBlock++) {
    uint64_t safeBound = genBlockEvents(curBlock, dre);
    int safeBoundIndex = std::upper_bound(txs.begin(), txs.end(), std::make_pair(safeBound, Tx(0))) - txs.begin();
    for (int i = 0; i < PARALLEL; i++) {
      pool.submit([safeBoundIndex] { genTxEvents(safeBoundIndex); });
    }
    pool.wait_for_tasks();
    boost::sort::block_indirect_sort(events.begin(), events.end(), 16);
    std::pair<Event, std::vector<std::vector<Event>>> executableEvents = genEventTree();
    process(executableEvents.first);
    for (const std::vector<Event> &events : executableEvents.second) {
      pool.submit([&events] { process(events); });
    }
    pool.wait_for_tasks();
  }
  std::sort(events.begin(), events.end());
  process(events);
  if (verbosity) {
    outputTxs("txs.csv");
    outputBlocks("blocks.csv");
    outputTxLog("tx.csv");
    outputBlockLog("block.csv");
    uint64_t tx50 = 0, tx90 = 0, tx100 = 0;
    uint64_t block50 = 0, block90 = 0, block100 = 0;
    for (auto &[t50, t90, t100] : txLog) {
      tx50 += t50;
      tx90 += t90;
      tx100 += t100;
    }
    for (auto &[t50, t90, t100] : blockLog) {
      block50 += t50;
      block90 += t90;
      block100 += t100;
    }
    tx50 /= txLog.size();
    tx90 /= txLog.size();
    tx100 /= txLog.size();
    block50 /= blockLog.size();
    block90 /= blockLog.size();
    block100 /= blockLog.size();
    std::cout << "tx " << tx50 << " " << tx90 << " " << tx100 << std::endl;
    std::cout << "block " << block50 << " " << block90 << " " << block100 << std::endl;
  }
  for (Node *node : nodes)
    delete node;
}
