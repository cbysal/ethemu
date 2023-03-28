#include <iomanip>
#include <sstream>

#include "common/math.h"

std::string idToString(Id id) {
  std::stringstream ss;
  ss << "emu" << std::setfill('0') << std::setw(6) << id;
  return ss.str();
}

std::string hashHex(Hash hash) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(8) << hash;
  return ss.str();
}
