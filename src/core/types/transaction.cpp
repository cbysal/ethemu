#include "core/types/transaction.h"

Tx newTx(uint16_t from, uint16_t to, uint16_t nonce) { return (uint64_t(to) << 32) | (uint64_t(from) << 16) | nonce; }

Hash hashTx(Tx tx) { return tx & 0xffff; }
