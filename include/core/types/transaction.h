#pragma once

#include <cstdint>
#include <vector>

using Id = uint16_t;
using Tx = uint64_t;
using Hash = uint32_t;

class Block;

extern std::vector<std::pair<uint64_t, Tx>> txs;

void preGenTxs(std::vector<std::pair<uint64_t, Block *>> &blocks, uint64_t minTx, uint64_t maxTx, uint64_t prefill,
               int nodeNum);
Hash hashTx(Tx tx);
