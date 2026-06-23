#include "../include/bloom_filter.h"

#include <functional>

BloomFilter::BloomFilter(size_t bits) { bitset.resize(bits); }

size_t BloomFilter::Hash1(const std::string &key) const {
  return std::hash<std::string>{}(key) % bitset.size();
}

size_t BloomFilter::Hash2(const std::string &key) const {
  return (std::hash<std::string>{}(key) * 31) % bitset.size();
}

void BloomFilter::Add(const std::string &key) {
  bitset[Hash1(key)] = true;

  bitset[Hash2(key)] = true;
}

bool BloomFilter::MightContain(const std::string &key) const {
  return bitset[Hash1(key)] && bitset[Hash2(key)];
}