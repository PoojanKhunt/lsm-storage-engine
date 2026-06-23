// include/record.h
#pragma once

#include <string>

enum class RecordType { PUT, DELETE };

struct Record {
  std::string key;
  std::string value;
  uint64_t sequence_number;
  RecordType type;
};