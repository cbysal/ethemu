#include <csignal>
#include <iostream>

#include "cxxopts.hpp"
#include "emu/config.h"
#include "ethemu.h"
#include "event/block_timer_event.h"
#include "event/body_fetch_timer_event.h"
#include "event/header_fetch_timer_event.h"
#include "event/tx_timer_event.h"
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
                cxxopts::value<int>()->default_value("150"));
Option txMaxOpt("tx.max", "Restrict the maximum transaction num sent during each block time",
                cxxopts::value<int>()->default_value("170"));
Option blockTimeOpt("block.time", "Interval of consensus for blocks", cxxopts::value<int>()->default_value("30"));
Option simTimeOpt("sim.time", "Time limit of the simulation", cxxopts::value<int>()->default_value("1000000"));
Option verbosityOpt("verbosity", "Show all outputs if it is on", cxxopts::value<bool>());

std::vector<Option> opts = {datadirOpt,  nodesOpt, minersOption, peerMinOpt,   peerMaxOpt, delayMinOpt,
                            delayMaxOpt, txMinOpt, txMaxOpt,     blockTimeOpt, simTimeOpt, verbosityOpt};

int main(int argc, char *argv[]) {
  std::for_each(opts.begin(), opts.end(), [](auto opt) { options.add_option("", opt); });
  auto result = options.parse(argc, argv);
  switch (result.unmatched().size()) {
  case 0:
    ethemu(result["datadir"].as<std::string>(), result["sim.time"].as<int>(), result["verbosity"].as<bool>());
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

void ethemu(const std::string &dataDir, uint64_t simTime, bool verbosity) {
  loadConfig(dataDir);
  std::vector<std::unique_ptr<Node>> nodes;
  for (int i = 0; i < global.nodes.size(); i++) {
    const std::unique_ptr<EmuNode> &emuNode = global.nodes[i];
    std::unique_ptr<Node> node = std::make_unique<Node>(emuNode->id);
    nodes.push_back(std::move(node));
  }
  for (auto &node : nodes)
    for (auto peer : global.nodes[node->id]->peers)
      node->addPeer(nodes[peer]);
  std::shared_ptr<Block> genesisBlock = std::make_shared<Block>(0, 0, 0, std::vector<Tx>{});
  for (auto &node : nodes) {
    node->blocksByNumber[genesisBlock->number()] = genesisBlock;
    node->blocksByHash[genesisBlock->hash()] = genesisBlock;
  }
  std::priority_queue<Event *, std::vector<Event *>, CompareEvent> events;
  events.push(new TxTimerEvent(0));
  events.push(new BlockTimerEvent(0));
  for (auto &node : nodes) {
    events.push(new HeaderFetchTimerEvent(0, node->id));
    events.push(new BodyFetchTimerEvent(0, node->id));
  }
  while (!events.empty()) {
    Event *event = events.top();
    if (event->timestamp > simTime)
      break;
    events.pop();
    event->process(events, nodes);
    if (verbosity || typeid(*event) == typeid(BlockTimerEvent))
      std::cout << event->toString() << std::endl;
    delete event;
  }
  while (!events.empty()) {
    Event *event = events.top();
    events.pop();
    delete event;
  }
  std::cerr << "Mem: ";
  pid_t pid = getpid();
  system(std::string("cat /proc/" + std::to_string(pid) + "/status | grep VmHWM | awk '{print $2 $3}' > /dev/fd/2")
             .data());
}
