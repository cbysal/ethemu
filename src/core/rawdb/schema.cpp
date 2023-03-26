#include "core/rawdb/schema.h"

enum { blockKeyPrefix };

std::string blockKey(uint64_t id, uint64_t number) {
  std::string result;
  result.resize(17);
  char *resultPtr = result.data();
  *resultPtr = blockKeyPrefix;
  *((uint64_t *)(resultPtr + 1)) = id;
  *((uint64_t *)(resultPtr + 9)) = number;
  return result;
}