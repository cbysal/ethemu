#include <filesystem>
#include <memory>
#include <unordered_set>

#include "cxxopts.hpp"
#include "emu/config.h"
#include "emu/types.h"
#include "ethemu.h"

void setId(std::vector<std::unique_ptr<EmuNode>> &nodes) {
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->id = i;
}

void setMiners(const cxxopts::ParseResult &options, std::vector<std::unique_ptr<EmuNode>> &nodes) {
  int num = options["miners"].as<int>();
  if (nodes.size() <= num) {
    for (auto &node : nodes)
      node->isMiner = true;
    return;
  }
  std::unordered_set<int> miners;
  srand(time(nullptr));
  while (miners.size() < num)
    miners.insert(std::rand() % nodes.size());
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->isMiner = miners.count(i);
}

void setPeers(const cxxopts::ParseResult &options, std::vector<std::unique_ptr<EmuNode>> &nodes) {
  double density = (double)options["peers"].as<int>() / (nodes.size() - 1);
  int maxDist = nodes.size() / 2;
  for (int i = 0; i < nodes.size(); i++) {
    for (int j = i + 1; j < nodes.size(); j++) {
      int dis = abs(i - j);
      int theta = density >= (double)std::min<int>(dis, nodes.size() - dis) / maxDist;
      if (rand() % 100 / 100.0 < 0.25 * density + 0.75 * theta) {
        nodes[i]->peers.push_back(j);
        nodes[j]->peers.push_back(i);
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
  auto num = options["nodes"].as<int>();
  std::vector<std::unique_ptr<EmuNode>> nodes(num);
  for (int i = 0; i < num; i++) {
    nodes[i] = std::make_unique<EmuNode>();
  }
  setId(nodes);
  setMiners(options, nodes);
  setPeers(options, nodes);
  global.period = options["block.time"].as<int>();
  global.minDelay = options["delay.min"].as<int>();
  global.maxDelay = options["delay.max"].as<int>();
  global.minTx = options["tx.min"].as<int>();
  global.maxTx = options["tx.max"].as<int>();
  for (auto &node : nodes)
    global.nodes[node->id] = std::move(node);
  storeConfig(dataDir);
}
