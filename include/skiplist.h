#pragma once

#include "record.h"
#include <optional>
#include <string>
#include <vector>

constexpr int MAX_LEVEL = 16;

class SkipNode {
public:
  std::string key;
  Record record;
  std::vector<SkipNode *> forward;

  SkipNode(const std::string &key, const Record &record, int level);
};

class SkipList {
public:
  SkipList();
  ~SkipList();

  void Insert(const std::string &key, const Record &record);

  std::optional<Record> Search(const std::string &key) const;

  class Iterator {
  public:
    Iterator(SkipNode *node);

    bool Valid() const;

    void Next();

    const Record &Value() const;

  private:
    SkipNode *current;
  };

  Iterator Begin() const;

private:
  SkipNode *head;
  int current_level;

  int RandomLevel();
};