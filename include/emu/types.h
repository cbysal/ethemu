#pragma once

#include <vector>

#include "json.hpp"

using json = nlohmann::json;

using Id = uint16_t;

struct EmuNode {
  Id id;
  bool isMiner;
  std::vector<Id> peers;

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
