#pragma once

#include <string>
#include <vector>

#include "core/types/header.h"
#include "core/types/transaction.h"

class Block {
public:
  std::shared_ptr<Header> header;
  std::vector<Tx> txs;

private:
  Block(){};

  Hash txsHash(const std::vector<Tx> &txs) const {
    Hash hash = 0;
    int off = 0;
    for (Tx tx : txs) {
      Hash txHash = hashTx(tx);
      hash ^= (txHash << off) | (txHash >> (32 - off));
      off = (off + 1) % 32;
    }
    return hash;
  }

public:
  Block(const std::shared_ptr<Header> &header, const std::vector<Tx> &txs) : header(header), txs(txs) {}

  Block(Hash parentHash, Id coinbase, uint32_t number, const std::vector<Tx> &txs)
      : header(std::make_shared<Header>(parentHash, coinbase, number, txsHash(txs))), txs(txs) {}

  uint32_t number() const { return header->number; }

  Id coinbase() const { return header->coinbase; }

  Hash hash() const { return header->hash(); }
};