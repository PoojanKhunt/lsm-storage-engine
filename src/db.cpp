#include "../include/db.h"
#include "../include/sstable.h"
#include <algorithm>
#include <iostream>
#include <vector>

DB::DB() : wal("data.wal"), manifest("MANIFEST"), next_sequence(1) {
  active_memtable = std::make_unique<MemTable>();

  sstable_files = manifest.Load();

  // std::cout << "Loaded " << sstable_files.size() << " SSTables\n";

  auto records = wal.Recover();

  for (const auto &record : records) {
    active_memtable->PutRecord(record);

    next_sequence = std::max(next_sequence, record.sequence_number + 1);
  }
}

bool DB::Put(const std::string &key, const std::string &value) {
  Record record;

  record.key = key;
  record.value = value;
  record.sequence_number = next_sequence++;

  record.type = RecordType::PUT;

  wal.Append(record);

  active_memtable->PutRecord(record);

  if (active_memtable->Size() >= MEMTABLE_LIMIT) {
    FlushMemTable();
  }

  return true;
}

bool DB::Delete(const std::string &key) {
  Record record;

  record.key = key;
  record.value = "";

  record.sequence_number = next_sequence++;

  record.type = RecordType::DELETE;

  wal.Append(record);

  active_memtable->PutRecord(record);

  return true;
}

std::optional<std::string> DB::Get(const std::string &key) {
  // std::cout << "SSTables: " << sstable_files.size() << '\n';
  auto rec = active_memtable->Get(key);

  if (rec) {
    if (rec->type == RecordType::DELETE) {
      return std::nullopt;
    }

    return rec->value;
  }

  for (int i = (int)sstable_files.size() - 1; i >= 0; i--) {
    const auto &meta = sstable_files[i];

    if (key < meta.min_key)
      continue;

    if (key > meta.max_key)
      continue;
    // std::cout << "Searching " << meta.filename << '\n';
    SSTable table(meta.filename);

    auto disk_rec = table.Get(key);

    if (disk_rec) {
      if (disk_rec->type == RecordType::DELETE) {
        return std::nullopt;
      }

      return disk_rec->value;
    }
  }

  return std::nullopt;
}

void DB::FlushMemTable() {
  immutable_memtable = std::move(active_memtable);

  active_memtable = std::make_unique<MemTable>();

  std::string filename =
      "sst_" + std::to_string(sstable_files.size() + 1) + ".dat";

  SSTable table(filename);

  auto meta = table.Flush(*immutable_memtable);

  SSTableMeta info{filename, meta.min_key, meta.max_key};

  sstable_files.push_back(info);

  manifest.AddSSTable(info);

  immutable_memtable.reset();
}