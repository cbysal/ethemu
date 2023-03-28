#include "emu/config.h"
#include "common/math.h"

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
    uint64_t id = std::stoi(key);
    std::unique_ptr<EmuNode> node = std::make_unique<EmuNode>();
    node->id = value["id"];
    node->isMiner = value["isMiner"];
    json::array_t peersJson;
    for (auto p : value["peers"]) {
      uint64_t peer = p;
      node->peers.push_back(peer);
    }
    global.nodes[id] = std::move(node);
  }
}

void storeConfig(const std::string &dataDir) {
  json data;
  data["period"] = global.period;
  data["minDelay"] = global.minDelay;
  data["maxDelay"] = global.maxDelay;
  data["minTxInterval"] = global.minTxInterval;
  data["maxTxInterval"] = global.maxTxInterval;
  for (auto &[id, node] : global.nodes) {
    json nodeJson = node->toJson();
    data["nodes"][std::to_string(id)] = nodeJson;
  }
  const std::string configPath = std::filesystem::path(dataDir) / CONFIG_JSON;
  std::ofstream ofs(configPath);
  ofs << data.dump(4);
  ofs.close();
}
