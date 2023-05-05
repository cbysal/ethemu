#pragma once

#include <vector>

#include "json.hpp"

using json = nlohmann::json;

using Id = uint16_t;

struct EmuNode {
  Id id;
  std::vector<std::tuple<Id, uint16_t, uint16_t>> peers;

  json toJson() const {
    json data;
    data["id"] = id;
    json::array_t peersJson;
    for (const auto &peer : peers)
      peersJson.emplace_back(peer);
    data["peers"] = peersJson;
    return data;
  }
};
