#pragma once

#include <cstdint>

using Id = uint16_t;
using Tx = uint64_t;
using Hash = uint32_t;

Tx newTx(Id from, uint16_t nonce);
Hash hashTx(Tx tx);
