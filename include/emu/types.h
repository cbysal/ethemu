#pragma once

#include <cstdint>
#include <vector>

#include "json.hpp"

using json = nlohmann::json;

struct EmuNode {
  uint64_t id;
  bool isMiner;
  std::vector<uint64_t> peers;

  json toJson() const {
    json data;
    data["id"] = id;
    data["isMiner"] = isMiner;
    json::array_t peersJson;
    for (auto peer : peers)
      peersJson.push_back(peer);
    data["peers"] = peersJson;
    return data;
  }
};
