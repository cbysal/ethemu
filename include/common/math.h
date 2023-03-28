#pragma once

#include <string>

using Id = uint16_t;
using Hash = uint32_t;

std::string idToString(Id);

std::string hashHex(Hash);
