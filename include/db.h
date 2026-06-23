#pragma once

#include "manifest.h"
#include "memtable.h"
#include "sstable_meta.h"
#include "wal.h"

#include <memory>
#include <vector>

constexpr size_t MEMTABLE_LIMIT = 100;

class DB {
public:
  DB();

  bool Put(const std::string &key, const std::string &value);

  std::optional<std::string> Get(const std::string &key);

  bool Delete(const std::string &key);

private:
  void FlushMemTable();

  std::unique_ptr<MemTable> active_memtable;

  std::unique_ptr<MemTable> immutable_memtable;

  WAL wal;

  uint64_t next_sequence;

  std::vector<SSTableMeta> sstable_files;

  Manifest manifest;
};