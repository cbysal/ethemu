#include <atomic>
#include <csignal>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

#include "core/types/transaction.h"
#include "cxxopts.hpp"
#include "emu/config.h"
#include "ethemu.h"
#include "event/block_event.h"
#include "event/tx_event.h"
#include "node/node.h"

using Options = cxxopts::Options;
using Option = cxxopts::Option;

Options options("Ethemu");

Option datadirOpt("datadir", "Data directory for the databases and keystore", cxxopts::value<std::string>());
Option nodesOpt("nodes", "Number of emulated nodes", cxxopts::value<int>()->default_value("100"));
Option minersOption("miners", "Number of emulated miners", cxxopts::value<int>()->default_value("20"));
Option peerMinOpt("peer.min", "Restrict the minimum peer num for each node",
                  cxxopts::value<int>()->default_value("20"));
Option peerMaxOpt("peer.max", "Restrict the maximum peer num for each node",
                  cxxopts::value<int>()->default_value("50"));
Option delayMinOpt("delay.min", "Minimum delay between peers", cxxopts::value<int>()->default_value("500"));
Option delayMaxOpt("delay.max", "Maximum delay between peers", cxxopts::value<int>()->default_value("1000"));
Option txMinOpt("tx.min", "Restrict the minimum transaction num in a block",
                cxxopts::value<int>()->default_value("150"));
Option txMaxOpt("tx.max", "Restrict the maximum transaction num in a block",
                cxxopts::value<int>()->default_value("160"));
Option blockTimeOpt("block.time", "Interval of consensus for blocks", cxxopts::value<int>()->default_value("15000"));
Option prefillOpt("prefill", "Fill transaction pool with transactions before emulation",
                  cxxopts::value<int>()->default_value("0"));
Option simTimeOpt("sim.time", "Time limit of the simulation", cxxopts::value<int>()->default_value("1000000"));
Option verbosityOpt("verbosity", "Show all outputs if it is on", cxxopts::value<bool>());

std::vector<Option> opts = {datadirOpt, nodesOpt, minersOption, peerMinOpt, peerMaxOpt, delayMinOpt, delayMaxOpt,
                            txMinOpt,   txMaxOpt, blockTimeOpt, simTimeOpt, prefillOpt, verbosityOpt};

std::vector<std::pair<uint16_t, uint64_t>>
dijkstra(const std::vector<std::vector<std::pair<uint16_t, uint64_t>>> &graph, Id from) {
  std::vector<bool> visited(graph.size(), false);
  std::priority_queue<std::pair<uint64_t, uint16_t>, std::vector<std::pair<uint64_t, uint16_t>>,
                      std::greater<std::pair<uint64_t, uint16_t>>>
      pq;
  std::vector<std::pair<uint16_t, uint64_t>> result(graph.size(), {UINT16_MAX, UINT64_MAX});
  result[from] = {from, 0};
  pq.push(std::make_pair(0, from));
  while (!pq.empty()) {
    Id from = pq.top().second;
    pq.pop();
    if (visited[from])
      continue;
    visited[from] = true;
    for (auto &[to, len] : graph[from]) {
      if (result[to].second > result[from].second + len) {
        result[to].second = result[from].second + len;
        result[to].first = from;
        pq.push(std::make_pair(result[to].second, to));
      }
    }
  }
  return result;
}

const int PARALLEL = 16;
std::vector<Node *> nodes;
std::vector<BlockEvent *> blockEvents;
std::atomic_int blockEventId = 0;
std::mutex blockMu;
std::atomic_int txEventId = 0;
std::vector<TxEvent *> txEvents;
std::mutex txMu;

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
  int curId;
  while ((curId = blockEventId++) < blocks.size()) {
    auto &[blockTime, block] = blocks[curId];
    std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays;
    delays.reserve(nodes.size());
    for (Node *node : nodes) {
      std::vector<std::pair<uint16_t, uint64_t>> delay(node->peerList.size());
      int n = std::sqrt(node->peerList.size());
      for (int i = 0; i < n; i++)
        delay[i] = {node->peerList[i], global.minDelay + dre() % (global.maxDelay - global.minDelay)};
      for (int i = n; i < node->peerList.size(); i++)
        delay[i] = {node->peerList[i],
                    (global.minDelay + dre() % (global.maxDelay - global.minDelay)) * 5 + dre() % 500};
      delays.push_back(delay);
    }
    std::vector<std::pair<uint16_t, uint64_t>> timeSpent = dijkstra(delays, block->coinbase);
    blockMu.lock();
    for (Id to = 0; to < nodes.size(); to++) {
      Id from = timeSpent[to].first;
      uint64_t dly = timeSpent[to].second;
      blockEvents.push_back(new BlockEvent(blockTime + dly, from, to, block));
    }
    blockMu.unlock();
  }
}

void genTxEvents() {
  std::random_device rd;
  std::default_random_engine dre(rd());
  std::vector<std::vector<std::pair<uint16_t, uint64_t>>> delays(nodes.size());
  int curId;
  while ((curId = txEventId++) < txs.size()) {
    auto &[txTime, tx] = txs[curId];
    for (Id i = 0; i < nodes.size(); i++) {
      Node *node = nodes[i];
      std::vector<std::pair<uint16_t, uint64_t>> &delay = delays[i];
      delay.resize(node->peerList.size());
      int n = std::sqrt(delay.size());
      for (int j = 0; j < n; j++)
        delay[j] = {node->peerList[j], global.minDelay + dre() % (global.maxDelay - global.minDelay)};
      for (int j = n; j < delay.size(); j++)
        delay[j] = {node->peerList[j], (global.minDelay + dre() % (global.maxDelay - global.minDelay)) * 3};
    }
    std::vector<std::pair<uint16_t, uint64_t>> timeSpent = dijkstra(delays, (Id)(tx >> 16));
    txMu.lock();
    for (Id to = 0; to < nodes.size(); to++) {
      Id from = timeSpent[to].first;
      uint64_t dly = timeSpent[to].second;
      txEvents.push_back(new TxEvent(txTime + dly, from, to, tx));
    }
    txMu.unlock();
  }
}

void ethemu(const std::string &dataDir, uint64_t simTime, uint64_t prefill, bool verbosity) {
  loadConfig(dataDir);
  for (int i = 0; i < global.nodes.size(); i++) {
    const std::unique_ptr<EmuNode> &emuNode = global.nodes[i];
    Node *node = new Node(emuNode->id);
    nodes.push_back(node);
  }
  for (auto &node : nodes)
    for (auto peer : global.nodes[node->id]->peers)
      node->addPeer(nodes[peer]);
  preGenBlocks(simTime, 12000, 15000, nodes.size());
  blockEvents.reserve(blocks.size() * nodes.size());
  std::vector<std::thread> genBlockThreads(PARALLEL);
  for (int i = 0; i < PARALLEL; i++) {
    genBlockThreads[i] = std::thread(genBlockEvents);
  }
  for (std::thread &genBlockThread : genBlockThreads)
    genBlockThread.join();
  std::sort(blockEvents.begin(), blockEvents.end(),
            [](BlockEvent *e1, BlockEvent *e2) { return e1->timestamp < e2->timestamp; });
  preGenTxs(blocks, global.minTx, global.maxTx, prefill, nodes.size());
  txEvents.reserve(txs.size() * nodes.size());
  std::vector<std::thread> genTxThreads(PARALLEL);
  for (int i = 0; i < PARALLEL; i++) {
    genTxThreads[i] = std::thread(genTxEvents);
  }
  for (std::thread &genTxThread : genTxThreads)
    genTxThread.join();
  std::sort(txEvents.begin(), txEvents.end(), [](TxEvent *e1, TxEvent *e2) { return e1->timestamp < e2->timestamp; });
  for (auto &node : nodes)
    node->setTxNum(txs.size());
  int curTx = 0;
  for (BlockEvent *blockEvent : blockEvents) {
    uint64_t curTime = blockEvent->timestamp;
    while (curTx < txEvents.size() && txEvents[curTx]->timestamp < curTime) {
      txEvents[curTx]->process(nodes);
      curTx++;
    }
    blockEvent->process(nodes);
    if (verbosity)
      std::cout << blockEvent->toString() << std::endl;
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
