#include <filesystem>
#include <random>
#include <unordered_set>

#include "cxxopts.hpp"
#include "emu/config.h"
#include "emu/types.h"
#include "ethemu.h"

void setId(std::vector<EmuNode> &nodes) {
  for (int i = 0; i < nodes.size(); i++)
    nodes[i].id = i;
}

void setPeers(const cxxopts::ParseResult &options, std::vector<EmuNode> &nodes, std::default_random_engine &dre) {
  double density = (double)options["peers"].as<int>() / (nodes.size() - 1);
  int minDelay = options["delay.min"].as<int>();
  int maxDelay = options["delay.max"].as<int>();
  int minBw = options["bw.min"].as<int>();
  int maxBw = options["bw.max"].as<int>();
  int maxDist = nodes.size() / 2;
  for (int i = 0; i < nodes.size(); i++) {
    for (int j = i + 1; j < nodes.size(); j++) {
      int delay = minDelay + dre() % (maxDelay - minDelay);
      int bw = minBw + dre() % (maxBw - minBw);
      int dis = abs(i - j);
      int theta = density >= (double)std::min<int>(dis, nodes.size() - dis) / maxDist;
      if (dre() % 100 / 100.0 < 0.25 * density + 0.75 * theta) {
        nodes[i].peers.emplace_back(j, delay, bw);
        nodes[j].peers.emplace_back(i, delay, bw);
      }
    }
  }
}

void gen(const cxxopts::ParseResult &options) {
  auto dataDir = options["datadir"].as<std::string>();
  if (std::filesystem::exists(dataDir)) {
    std::filesystem::remove_all(dataDir);
  }
  std::filesystem::create_directories(dataDir);
  std::random_device rd;
  std::default_random_engine dre(rd());
  auto num = options["nodes"].as<int>();
  std::vector<EmuNode> nodes(num);
  setId(nodes);
  setPeers(options, nodes, dre);
  global.blockTime = options["block.time"].as<int>();
  global.minDelay = options["delay.min"].as<int>();
  global.maxDelay = options["delay.max"].as<int>();
  global.minTx = options["tx.min"].as<int>();
  global.maxTx = options["tx.max"].as<int>();
  global.minBlockSize = options["block.min"].as<int>();
  global.maxBlockSize = options["block.max"].as<int>();
  for (auto &node : nodes)
    global.nodes[node.id] = std::move(node);
  storeConfig(dataDir);
}
