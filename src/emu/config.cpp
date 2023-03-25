#include "emu/config.h"

Config global;

void loadConfig(const std::string &dataDir) {
  const std::string configPath = std::filesystem::path(dataDir) / CONFIG_JSON;
  std::ifstream ifs(configPath);
  json data = json::parse(ifs);
  ifs.close();
  global.period = data["period"];
  global.minDelay = data["minDelay"];
  global.maxDelay = data["maxDelay"];
  global.minTxInterval = data["minTxInterval"];
  global.maxTxInterval = data["maxTxInterval"];
  for (auto [key, value] : data["nodes"].items()) {
    Address addr;
    addr.parseFromString(key);
    EmuNode *node = new EmuNode();
    node->id = value["id"];
    node->addr.parseFromString(value["addr"]);
    node->isMiner = value["isMiner"];
    json::array_t peersJson;
    for (auto p : value["peers"]) {
      Address peer;
      peer.parseFromString(p);
      node->peers.push_back(peer);
    }
    global.nodes[addr] = node;
  }
}

void storeConfig(const std::string &dataDir) {
  json data;
  data["period"] = global.period;
  data["minDelay"] = global.minDelay;
  data["maxDelay"] = global.maxDelay;
  data["minTxInterval"] = global.minTxInterval;
  data["maxTxInterval"] = global.maxTxInterval;
  for (auto node : global.nodes) {
    json nodeJson = node.second->toJson();
    data["nodes"][node.first.toString()] = nodeJson;
  }
  const std::string configPath = std::filesystem::path(dataDir) / CONFIG_JSON;
  std::ofstream ofs(configPath);
  ofs << data.dump(4);
  ofs.close();
}
