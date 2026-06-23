#pragma once

#include "sstable_meta.h"

#include <string>
#include <vector>

class Manifest {
public:
  explicit Manifest(const std::string &filename);

  void AddSSTable(const SSTableMeta &meta);

  std::vector<SSTableMeta> Load();

private:
  std::string filename;
};