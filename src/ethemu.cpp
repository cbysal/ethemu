#include <csignal>
#include <iostream>

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

void ethemu(const std::string &dataDir, uint64_t simTime, uint64_t prefill, bool verbosity) {
  loadConfig(dataDir);
  std::vector<Node *> nodes;
  for (int i = 0; i < global.nodes.size(); i++) {
    const std::unique_ptr<EmuNode> &emuNode = global.nodes[i];
    Node *node = new Node(emuNode->id);
    nodes.push_back(node);
  }
  for (auto &node : nodes)
    for (auto peer : global.nodes[node->id]->peers)
      node->addPeer(nodes[peer]);
  preGenBlocks(simTime, 12000, 15000, nodes.size());
  preGenTxs(blocks, global.minTx, global.maxTx, prefill, nodes.size());
  for (auto &node : nodes)
    node->setTxNum(txs.size());
  std::priority_queue<Event *, std::vector<Event *>, CompareEvent> events;
  int curTx = 0;
  for (auto &[curTime, block] : blocks) {
    while (curTx < txs.size() && txs[curTx].first < curTime) {
      auto &[txTime, tx] = txs[curTx];
      Id from = tx >> 16;
      Event *event = new TxEvent(txTime, from, from, false, tx);
      events.push(event);
      curTx++;
    }
    events.push(new BlockEvent(curTime, block->coinbase, block->coinbase, false, block));
    while (!events.empty() && events.top()->timestamp <= curTime) {
      Event *event = events.top();
      if (event->timestamp > simTime)
        break;
      events.pop();
      event->process(events, nodes);
      if (verbosity)
        std::cout << "Events: " << events.size() << " " << event->toString() << std::endl;
      delete event;
    }
  }
  while (!events.empty()) {
    Event *event = events.top();
    events.pop();
    delete event;
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
