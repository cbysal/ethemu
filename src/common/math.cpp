#include <iomanip>
#include <sstream>

#include "common/math.h"

std::string idToString(uint64_t id) {
  std::stringstream ss;
  ss << "emu" << std::setfill('0') << std::setw(6) << id;
  return ss.str();
}

std::string u64ToHex(uint64_t value) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(8) << value;
  return ss.str();
}
