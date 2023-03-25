#include "core/rawdb/schema.h"
#include "common/math.h"

enum { blockKeyPrefix };

std::string blockKey(std::string id, uint64_t number) {
  std::string result;
  result += char(blockKeyPrefix);
  result += id;
  result += u64ToString(number);
  return result;
}