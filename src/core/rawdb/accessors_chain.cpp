#include <iostream>

#include "core/rawdb/accessors_chain.h"
#include "core/rawdb/schema.h"

Block *readBlock(leveldb::DB *db, const std::string &id, uint64_t number) {
  std::string value;
  auto s = db->Get(leveldb::ReadOptions(), blockKey(id, number), &value);
  if (s.IsNotFound())
    return nullptr;
  return Block::parse(value);
}

void writeBlock(leveldb::DB *db, const std::string &id, Block *block) {
  std::string value = block->bytes();
  auto s = db->Put(leveldb::WriteOptions(), blockKey(id, block->number), value);
  if (!s.ok())
    throw s.ToString();
}
