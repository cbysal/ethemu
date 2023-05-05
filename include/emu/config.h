#pragma once

#include <unordered_map>

#include "emu/types.h"

using Id = uint16_t;

const std::string CONFIG_JSON = "config.json";

struct Config {
  uint64_t blockTime;
  uint64_t minDelay, maxDelay;
  uint64_t minTx, maxTx;
  uint16_t minBlockSize, maxBlockSize;
  std::unordered_map<Id, EmuNode> nodes;
};

extern Config global;

void loadConfig(const std::string &);

void storeConfig(const std::string &);
