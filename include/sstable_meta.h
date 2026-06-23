#pragma once

#include <string>

struct SSTableMeta {
  std::string filename;

  std::string min_key;
  std::string max_key;
};