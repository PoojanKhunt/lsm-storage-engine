#include "../include/wal.h"

#include <cstdint>
#include <fstream>

WAL::WAL(const std::string &filename) : filename(filename) {
  out.open(filename, std::ios::binary | std::ios::app);
}

WAL::~WAL() {
  if (out.is_open()) {
    out.close();
  }
}

void WAL::Append(const Record &record) {
  uint64_t seq = record.sequence_number;

  uint32_t type = static_cast<uint32_t>(record.type);

  uint32_t key_size = record.key.size();

  uint32_t value_size = record.value.size();

  out.write(reinterpret_cast<char *>(&seq), sizeof(seq));

  out.write(reinterpret_cast<char *>(&type), sizeof(type));

  out.write(reinterpret_cast<char *>(&key_size), sizeof(key_size));

  out.write(record.key.data(), key_size);

  out.write(reinterpret_cast<char *>(&value_size), sizeof(value_size));

  out.write(record.value.data(), value_size);

  out.flush();
}

std::vector<Record> WAL::Recover() {
  std::vector<Record> records;

  std::ifstream in(filename, std::ios::binary);

  while (true) {
    Record record;

    uint64_t seq;
    uint32_t type;
    uint32_t key_size;
    uint32_t value_size;

    if (!in.read(reinterpret_cast<char *>(&seq), sizeof(seq))) {
      break;
    }

    if (!in.read(reinterpret_cast<char *>(&type), sizeof(type))) {
      break;
    }

    if (!in.read(reinterpret_cast<char *>(&key_size), sizeof(key_size))) {
      break;
    }

    record.key.resize(key_size);

    if (!in.read(record.key.data(), key_size)) {
      break;
    }

    if (!in.read(reinterpret_cast<char *>(&value_size), sizeof(value_size))) {
      break;
    }

    record.value.resize(value_size);

    if (!in.read(record.value.data(), value_size)) {
      break;
    }

    record.sequence_number = seq;

    record.type = static_cast<RecordType>(type);

    records.push_back(record);
  }

  return records;
}