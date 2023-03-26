#include "core/rawdb/schema.h"

enum { blockKeyByNumberPrefix, blockKeyByHashPrefix };

std::string blockKeyByNumber(uint64_t id, uint64_t number) {
  std::string result;
  result.resize(17);
  char *resultPtr = result.data();
  *resultPtr = blockKeyByNumberPrefix;
  *((uint64_t *)(resultPtr + 1)) = id;
  *((uint64_t *)(resultPtr + 9)) = number;
  return result;
}

std::string blockKeyByHash(uint64_t id, uint64_t hash) {
  std::string result;
  result.resize(17);
  char *resultPtr = result.data();
  *resultPtr = blockKeyByHashPrefix;
  *((uint64_t *)(resultPtr + 1)) = id;
  *((uint64_t *)(resultPtr + 9)) = hash;
  return result;
}
