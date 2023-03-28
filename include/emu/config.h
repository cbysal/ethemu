#pragma once

#include <unordered_map>

#include "emu/types.h"

using Id = uint16_t;

const std::string CONFIG_JSON = "config.json";

struct Config {
  uint64_t period;
  uint64_t minDelay, maxDelay;
  uint64_t minTxInterval, maxTxInterval;
  std::unordered_map<Id, std::unique_ptr<EmuNode>> nodes;
};

extern Config global;

void loadConfig(const std::string &);

void storeConfig(const std::string &);
