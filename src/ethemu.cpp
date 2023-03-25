#include <iostream>
#include <queue>

#include "cxxopts.hpp"
#include "emu/config.h"
#include "ethemu.h"
#include "event/block_timer_event.h"
#include "event/event.h"
#include "event/tx_timer_event.h"
#include "leveldb/db.h"
#include "node/node.h"

using Options = cxxopts::Options;
using Option = cxxopts::Option;

Options options("Ethemu");

Option datadirOpt("datadir", "Data directory for the databases and keystore", cxxopts::value<std::string>());
Option nodesOpt("nodes", "Number of emulated nodes", cxxopts::value<int>()->default_value("30"));
Option minersOption("miners", "Number of emulated miners", cxxopts::value<int>()->default_value("5"));
Option peerMinOpt("peer.min", "Restrict the minimum peer num for each node",
                  cxxopts::value<int>()->default_value("20"));
Option peerMaxOpt("peer.max", "Restrict the maximum peer num for each node",
                  cxxopts::value<int>()->default_value("50"));
Option delayMinOpt("delay.min", "Minimum delay between peers", cxxopts::value<int>()->default_value("500"));
Option delayMaxOpt("delay.max", "Maximum delay between peers", cxxopts::value<int>()->default_value("1000"));
Option txMinOpt("tx.min", "Restrict the minimum transaction num sent during each block time",
                cxxopts::value<int>()->default_value("30"));
Option txMaxOpt("tx.max", "Restrict the maximum transaction num sent during each block time",
                cxxopts::value<int>()->default_value("50"));
Option blockTimeOpt("block.time", "Interval of consensus for blocks", cxxopts::value<int>()->default_value("30"));

std::vector<Option> opts = {datadirOpt,  nodesOpt,    minersOption, peerMinOpt, peerMaxOpt,
                            delayMinOpt, delayMaxOpt, txMinOpt,     txMaxOpt,   blockTimeOpt};

int main(int argc, char *argv[]) {
  std::for_each(opts.begin(), opts.end(), [](auto opt) { options.add_option("", opt); });
  auto result = options.parse(argc, argv);
  switch (result.unmatched().size()) {
  case 0:
    ethemu(result["datadir"].as<std::string>());
    break;
  case 1: {
    auto subcmd = result.unmatched()[0];
    if (subcmd == "gen") {
      gen(result);
      break;
    }
    if (subcmd == "init") {
      init(result["datadir"].as<std::string>());
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

void ethemu(const std::string &dataDir) {
  loadConfig(dataDir);
  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;
  auto s = leveldb::DB::Open(options, dataDir, &db);
  if (!s.ok()) {
    throw s.ToString();
  }
  std::vector<uint64_t> nodeList;
  std::unordered_map<uint64_t, Node *> nodeMap;
  for (auto [addr, n] : global.nodes) {
    char id[9];
    sprintf(id, "emu%06d", n->id);
    Node *node = new Node(id, addr, db);
    nodeList.push_back(addr);
    nodeMap[addr] = node;
  }
  for (auto [addr, node] : nodeMap)
    for (auto peer : global.nodes[addr]->peers)
      node->addPeer(nodeMap[peer]);
  std::priority_queue<Event *, std::vector<Event *>, CompareEvent> events;
  events.push(new TxTimerEvent(0));
  events.push(new BlockTimerEvent(0));
  while (!events.empty()) {
    Event *event = events.top();
    events.pop();
    event->process(events, db, nodeList, nodeMap);
    std::cout << event->toString() << std::endl;
  }
  delete db;
}
