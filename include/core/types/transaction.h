#pragma once

#include <cstdint>
#include <vector>

using Id = uint16_t;
using Tx = uint64_t;
using Hash = uint32_t;

extern std::vector<std::pair<uint64_t, Tx>> txs;

void preGenTxs(uint64_t simTime, uint64_t minTxInterval, uint64_t maxTxInterval, uint64_t prefill, int nodeNum);
Hash hashTx(Tx tx);
