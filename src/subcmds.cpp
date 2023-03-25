#include <filesystem>
#include <iostream>
#include <unordered_set>

#include "core/rawdb/accessors_chain.h"
#include "core/types/block.h"
#include "cxxopts.hpp"
#include "emu/config.h"
#include "emu/types.h"
#include "ethemu.h"
#include "leveldb/db.h"

void setId(std::vector<EmuNode *> &nodes) {
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->id = i;
}

Address randAddr(int seed) {
  std::srand(seed);
  Address addr;
  for (unsigned char &b : addr.data)
    b = std::rand();
  return addr;
}

void setAddr(std::vector<EmuNode *> &nodes) {
  for (int i = 0; i < nodes.size(); i++) {
    nodes[i]->addr = randAddr(std::time(nullptr) + i);
  }
}

void setMiners(const cxxopts::ParseResult &options, std::vector<EmuNode *> &nodes) {
  int num = options["miners"].as<int>();
  std::unordered_set<int> miners;
  while (miners.size() < num)
    miners.insert(std::rand() % nodes.size());
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->isMiner = miners.count(i);
}

void setPeers(const cxxopts::ParseResult &options, std::vector<EmuNode *> &nodes) {
  int minPeer = options["peer.min"].as<int>();
  int maxPeer = options["peer.max"].as<int>();
  std::vector<std::unordered_set<int>> peers(nodes.size());
  bool toContinue;
  do {
    toContinue = false;

    for (int i = 0; i < nodes.size(); i++) {
      while (peers[i].size() < minPeer) {
        int j = std::rand() % nodes.size();
        if (i == j || peers[j].size() >= maxPeer)
          continue;
        peers[i].insert(j);
        peers[j].insert(i);
      }
    }

    for (const auto &node : peers) {
      if (node.size() < minPeer || node.size() > maxPeer) {
        toContinue = true;
        break;
      }
    }
  } while (toContinue);
  for (int i = 0; i < nodes.size(); i++) {
    for (int j : peers[i]) {
      nodes[i]->peers.push_back(nodes[j]->addr);
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
  std::vector<EmuNode *> nodes(num);
  for (int i = 0; i < num; i++) {
    nodes[i] = new EmuNode();
  }
  setId(nodes);
  setAddr(nodes);
  setMiners(options, nodes);
  setPeers(options, nodes);
  global.period = options["block.time"].as<int>();
  global.minDelay = options["delay.min"].as<int>();
  global.maxDelay = options["delay.max"].as<int>();
  global.minTxInterval = global.period * 1000 / options["tx.max"].as<int>();
  global.maxTxInterval = global.period * 1000 / options["tx.min"].as<int>();
  for (auto node : nodes) {
    global.nodes[node->addr] = node;
  }
  storeConfig(dataDir);
}

void init(const std::string &dataDir) {
  loadConfig(dataDir);
  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;
  auto s = leveldb::DB::Open(options, dataDir, &db);
  if (!s.ok())
    throw s.ToString();
  Block *block = new Block();
  for (auto [addr, node] : global.nodes) {
    char id[9];
    std::sprintf(id, "emu%06d", node->id);
    writeBlock(db, id, block);
  }
  delete db;
}
