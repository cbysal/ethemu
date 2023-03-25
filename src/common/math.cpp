#include "common/math.h"

std::string u64ToString(uint64_t value) {
  std::string str;
  str.resize(8);
  for (int i = 0; i < 8; i++)
    str[i] = (value >> ((7 - i) * 8)) & 0xff;
  return str;
}

uint64_t u64FromString(const std::string &data) {
  uint64_t value = 0;
  for (int i = 0; i < 8; i++)
    value |= uint64_t(data[i]) << (7 - i) * 8;
  return value;
}