#pragma once

#include <string>

std::string idToString(uint64_t);

std::string u64ToHex(uint64_t);

std::string u64ToBytes(uint64_t);

uint64_t u64FromBytes(const std::string &);