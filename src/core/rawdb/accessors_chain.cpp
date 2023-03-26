#include "core/rawdb/accessors_chain.h"
#include "core/rawdb/schema.h"

std::unique_ptr<Block> readBlockByNumber(leveldb::DB *db, uint64_t id, uint64_t number) {
  std::string value;
  auto s = db->Get(leveldb::ReadOptions(), blockKeyByNumber(id, number), &value);
  if (s.IsNotFound())
    return nullptr;
  return Block::parse(value);
}

std::shared_ptr<Block> readBlockByHash(leveldb::DB *db, uint64_t id, uint64_t hash) {
  std::string value;
  auto s = db->Get(leveldb::ReadOptions(), blockKeyByHash(id, hash), &value);
  if (s.IsNotFound())
    return nullptr;
  return Block::parse(value);
}

void writeBlock(leveldb::DB *db, uint64_t id, const std::shared_ptr<Block> &block) {
  std::string value = block->bytes();
  auto s = db->Put(leveldb::WriteOptions(), blockKeyByNumber(id, block->number()), value);
  if (!s.ok())
    throw s.ToString();
  s = db->Put(leveldb::WriteOptions(), blockKeyByHash(id, block->hash()), value);
  if (!s.ok())
    throw s.ToString();
}
