#pragma once

#include <cstdint>
#include <vector>

#include "common/types.h"
#include "json.hpp"

using json = nlohmann::json;

struct EmuNode {
  uint64_t id;
  Address addr;
  bool isMiner;
  std::vector<Address> peers;

  json toJson() const {
    json data;
    data["id"] = id;
    data["addr"] = addr.toString();
    data["isMiner"] = isMiner;
    json::array_t peersJson;
    for (auto peer : peers)
      peersJson.push_back(peer.toString());
    data["peers"] = peersJson;
    return data;
  }
};