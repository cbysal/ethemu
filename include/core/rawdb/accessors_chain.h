#pragma once

#include <memory>

#include "core/types/block.h"
#include "leveldb/db.h"

std::unique_ptr<Block> readBlock(leveldb::DB *, uint64_t, uint64_t);
void writeBlock(leveldb::DB *, uint64_t, const std::shared_ptr<Block> &);
