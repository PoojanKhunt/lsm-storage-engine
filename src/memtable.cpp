#include "../include/memtable.h"

MemTable::MemTable() : next_sequence(1), current_size(0) {}

void MemTable::Put(const std::string &key, const std::string &value) {

  Record record;

  record.key = key;
  record.value = value;
  record.sequence_number = next_sequence++;

  record.type = RecordType::PUT;

  current_size += key.size() + value.size();

  table.Insert(key, record);
}

void MemTable::Delete(const std::string &key) {

  Record tombstone;

  tombstone.key = key;
  tombstone.value = "";

  tombstone.sequence_number = next_sequence++;

  tombstone.type = RecordType::DELETE;
  current_size += key.size();
  table.Insert(key, tombstone);
}

std::optional<Record> MemTable::Get(const std::string &key) const {

  return table.Search(key);
}

SkipList::Iterator MemTable::Begin() const { return table.Begin(); }

void MemTable::PutRecord(const Record &record) {
  current_size += record.key.size() + record.value.size();

  table.Insert(record.key, record);
}

size_t MemTable::Size() const { return current_size; }