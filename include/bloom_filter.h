#pragma once

#include <string>
#include <vector>

class BloomFilter {
public:
  explicit BloomFilter(size_t bits = 8192);

  void Add(const std::string &key);

  bool MightContain(const std::string &key) const;

private:
  std::vector<bool> bitset;

  size_t Hash1(const std::string &key) const;

  size_t Hash2(const std::string &key) const;
};