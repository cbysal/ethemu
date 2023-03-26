#pragma once

#include <memory>

#include "core/types/block.h"
#include "leveldb/db.h"

std::unique_ptr<Block> readBlock(leveldb::DB *, const std::string &, uint64_t);
void writeBlock(leveldb::DB *, const std::string &, const std::shared_ptr<Block> &);
