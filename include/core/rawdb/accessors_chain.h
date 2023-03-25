#pragma once

#include "core/types/block.h"
#include "leveldb/db.h"

Block *readBlock(leveldb::DB *, const std::string &, uint64_t);
void writeBlock(leveldb::DB *, const std::string &, Block *);
