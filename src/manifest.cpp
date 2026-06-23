#include "../include/manifest.h"

#include <fstream>
#include <sstream>

Manifest::Manifest(const std::string &filename) : filename(filename) {}

void Manifest::AddSSTable(const SSTableMeta &meta) {
  std::ofstream out(filename, std::ios::app);

  out << meta.filename << " " << meta.min_key << " " << meta.max_key << "\n";
}

std::vector<SSTableMeta> Manifest::Load() {
  std::vector<SSTableMeta> result;

  std::ifstream in(filename);

  std::string file;
  std::string min_key;
  std::string max_key;

  while (in >> file >> min_key >> max_key) {
    result.push_back({file, min_key, max_key});
  }

  return result;
}