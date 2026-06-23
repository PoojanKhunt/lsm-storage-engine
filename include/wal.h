#pragma once

#include "record.h"

#include <fstream>
#include <string>
#include <vector>

class WAL {
public:
  explicit WAL(const std::string &filename);

  ~WAL();

  void Append(const Record &record);

  std::vector<Record> Recover();

private:
  std::string filename;
  std::ofstream out;
};