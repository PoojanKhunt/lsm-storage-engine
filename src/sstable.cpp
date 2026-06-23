#include "../include/sstable.h"

#include <algorithm>
#include <fstream>
#include <iostream>

constexpr int BLOCK_RECORDS = 4;

SSTable::SSTable(const std::string &filename) : filename(filename) {
  LoadIndex();
  RebuildBloom();
}

SSTableMetaData SSTable::Flush(const MemTable &memtable) {
  std::ofstream out(filename, std::ios::binary);

  SSTableMetaData meta;

  bool first = true;

  auto it = memtable.Begin();

  while (it.Valid()) {
    const Record &record = it.Value();

    if (first) {
      meta.min_key = record.key;
      first = false;
    }

    meta.max_key = record.key;

    uint64_t seq = record.sequence_number;
    uint32_t type = static_cast<uint32_t>(record.type);
    uint32_t key_size = record.key.size();
    uint32_t value_size = record.value.size();

    bloom.Add(record.key);

    out.write(reinterpret_cast<char *>(&seq), sizeof(seq));
    out.write(reinterpret_cast<char *>(&type), sizeof(type));
    out.write(reinterpret_cast<char *>(&key_size), sizeof(key_size));
    out.write(record.key.data(), key_size);
    out.write(reinterpret_cast<char *>(&value_size), sizeof(value_size));
    out.write(record.value.data(), value_size);

    it.Next();
  }

  out.close();

  return meta;
}

void SSTable::LoadIndex() {
  index.clear();

  std::ifstream in(filename, std::ios::binary);

  if (!in)
    return;

  uint64_t offset = 0;
  int count = 0;

  while (true) {
    uint64_t seq;
    uint32_t type;
    uint32_t key_size;
    uint32_t value_size;

    if (!in.read(reinterpret_cast<char *>(&seq), sizeof(seq)))
      break;

    if (!in.read(reinterpret_cast<char *>(&type), sizeof(type)))
      break;

    if (!in.read(reinterpret_cast<char *>(&key_size), sizeof(key_size)))
      break;

    std::string key(key_size, '\0');

    if (!in.read(key.data(), key_size))
      break;

    if (!in.read(reinterpret_cast<char *>(&value_size), sizeof(value_size)))
      break;

    in.seekg(value_size, std::ios::cur);

    if (count % BLOCK_RECORDS == 0) {
      index.push_back({key, offset});
    }

    offset += sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t) +
              key_size + sizeof(uint32_t) + value_size;

    count++;
    // std::cout << "INDEX " << key << " " << offset << '\n';
  }
}

std::optional<Record> SSTable::Get(const std::string &key) {

  if (!bloom.MightContain(key)) {
    // std::cout << "Bloom skip: " << filename << '\n';

    return std::nullopt;
  }

  std::ifstream in(filename, std::ios::binary);

  while (true) {
    Record record;

    uint64_t seq;
    uint32_t type;
    uint32_t key_size;
    uint32_t value_size;

    if (!in.read(reinterpret_cast<char *>(&seq), sizeof(seq)))
      break;

    if (!in.read(reinterpret_cast<char *>(&type), sizeof(type)))
      break;

    if (!in.read(reinterpret_cast<char *>(&key_size), sizeof(key_size)))
      break;

    record.key.resize(key_size);

    if (!in.read(record.key.data(), key_size))
      break;

    if (!in.read(reinterpret_cast<char *>(&value_size), sizeof(value_size)))
      break;

    record.value.resize(value_size);

    if (!in.read(record.value.data(), value_size))
      break;

    record.sequence_number = seq;
    record.type = static_cast<RecordType>(type);

    if (record.key == key)
      return record;
  }

  return std::nullopt;
}

void SSTable::RebuildBloom() {
  std::ifstream in(filename, std::ios::binary);

  if (!in)
    return;

  while (true) {
    uint64_t seq;
    uint32_t type;
    uint32_t key_size;
    uint32_t value_size;

    if (!in.read(reinterpret_cast<char *>(&seq), sizeof(seq)))
      break;

    if (!in.read(reinterpret_cast<char *>(&type), sizeof(type)))
      break;

    if (!in.read(reinterpret_cast<char *>(&key_size), sizeof(key_size)))
      break;

    std::string key(key_size, '\0');

    if (!in.read(key.data(), key_size))
      break;

    if (!in.read(reinterpret_cast<char *>(&value_size), sizeof(value_size)))
      break;

    in.seekg(value_size, std::ios::cur);

    bloom.Add(key);
  }
}