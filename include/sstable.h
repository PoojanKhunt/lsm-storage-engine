#pragma once

#include "block_index.h"
#include "memtable.h"
#include "record.h"

#include <optional>
#include <string>
#include <vector>

struct SSTableMetaData {
  std::string min_key;
  std::string max_key;
};

class SSTable {
public:
  explicit SSTable(const std::string &filename);

  SSTableMetaData Flush(const MemTable &memtable);

  std::optional<Record> Get(const std::string &key);

private:
  std::string filename;
  std::vector<BlockIndexEntry> index;
  void LoadIndex();
};