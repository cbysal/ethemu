#pragma once

#include <cstdint>

using Tx = uint64_t;
using Hash = uint32_t;

Tx newTx(uint16_t from, uint16_t to, uint16_t nonce);
Hash hashTx(Tx tx);
