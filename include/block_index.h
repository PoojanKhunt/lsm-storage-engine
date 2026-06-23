#pragma once

#include <cstdint>
#include <string>

struct BlockIndexEntry {
  std::string last_key;
  uint64_t offset;
};