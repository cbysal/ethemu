#include <fstream>

#include "emu/config.h"

Config global;

void loadConfig(const std::string &dataDir) {
  const std::string configPath = std::filesystem::path(dataDir) / CONFIG_JSON;
  std::ifstream ifs(configPath);
  json data = json::parse(ifs);
  ifs.close();
  global.blockTime = data["blockTime"];
  global.minDelay = data["minDelay"];
  global.maxDelay = data["maxDelay"];
  global.minTx = data["minTx"];
  global.maxTx = data["maxTx"];
  global.minBlockSize = data["minBlockSize"];
  global.maxBlockSize = data["maxBlockSize"];
  for (auto [key, value] : data["nodes"].items()) {
    Id id = std::stoi(key);
    global.nodes[id] = {};
    global.nodes[id].id = value["id"];
    json::array_t peersJson;
    for (auto &peer : value["peers"])
      global.nodes[id].peers.push_back(peer);
  }
}

void storeConfig(const std::string &dataDir) {
  json data;
  data["blockTime"] = global.blockTime;
  data["minDelay"] = global.minDelay;
  data["maxDelay"] = global.maxDelay;
  data["minTx"] = global.minTx;
  data["maxTx"] = global.maxTx;
  data["minBlockSize"] = global.minBlockSize;
  data["maxBlockSize"] = global.maxBlockSize;
  for (auto &[id, node] : global.nodes) {
    json nodeJson = node.toJson();
    data["nodes"][std::to_string(id)] = nodeJson;
  }
  const std::string configPath = std::filesystem::path(dataDir) / CONFIG_JSON;
  std::ofstream ofs(configPath);
  ofs << data.dump(4);
  ofs.close();
}
