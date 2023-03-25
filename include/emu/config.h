#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "common/types.h"
#include "emu/types.h"
#include "json.hpp"

using json = nlohmann::json;

const std::string CONFIG_JSON = "config.json";

struct Config {
  uint64_t period;
  uint64_t minDelay, maxDelay;
  uint64_t minTxInterval, maxTxInterval;
  std::unordered_map<uint64_t, EmuNode *> nodes;
};

extern Config global;

void loadConfig(const std::string &);

void storeConfig(const std::string &);
