#pragma once

#include "skiplist.h"

class MemTable {
public:
  MemTable();

  void Put(const std::string &key, const std::string &value);

  void Delete(const std::string &key);

  std::optional<Record> Get(const std::string &key) const;

  void PutRecord(const Record &record);

  SkipList::Iterator Begin() const;

  size_t Size() const;

private:
  SkipList table;

  uint64_t next_sequence;
  size_t current_size;
};