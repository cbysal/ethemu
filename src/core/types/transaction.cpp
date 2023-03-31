#include "core/types/transaction.h"

uint64_t txId = 0;

Tx newTx(Id from, uint16_t nonce) { return ((txId++) << 32) | (uint64_t(from) << 16) | nonce; }

Hash hashTx(Tx tx) { return tx & 0xffffffff; }
