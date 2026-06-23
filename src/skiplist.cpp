#include "../include/skiplist.h"

#include <cstdlib>
#include <ctime>

SkipNode::SkipNode(const std::string &key, const Record &record, int level)
    : key(key), record(record), forward(level, nullptr) {}

SkipList::SkipList() {
  std::srand(std::time(nullptr));

  Record dummy;
  dummy.sequence_number = 0;
  dummy.type = RecordType::PUT;

  head = new SkipNode("", dummy, MAX_LEVEL);

  current_level = 1;
}

SkipList::~SkipList() {

  SkipNode *curr = head;

  while (curr) {

    SkipNode *next = curr->forward[0];

    delete curr;

    curr = next;
  }
}

int SkipList::RandomLevel() {

  int level = 1;

  while ((std::rand() & 1) && level < MAX_LEVEL) {
    level++;
  }

  return level;
}

std::optional<Record> SkipList::Search(const std::string &key) const {

  SkipNode *curr = head;

  for (int lvl = current_level - 1; lvl >= 0; lvl--) {
    while (curr->forward[lvl] && curr->forward[lvl]->key < key) {
      curr = curr->forward[lvl];
    }
  }

  curr = curr->forward[0];

  if (curr && curr->key == key) {
    return curr->record;
  }

  return std::nullopt;
}

void SkipList::Insert(const std::string &key, const Record &record) {

  std::vector<SkipNode *> update(MAX_LEVEL);

  SkipNode *curr = head;

  for (int lvl = current_level - 1; lvl >= 0; lvl--) {
    while (curr->forward[lvl] && curr->forward[lvl]->key < key) {
      curr = curr->forward[lvl];
    }

    update[lvl] = curr;
  }

  curr = curr->forward[0];

  if (curr && curr->key == key) {
    curr->record = record;
    return;
  }

  int node_level = RandomLevel();

  if (node_level > current_level) {

    for (int i = current_level; i < node_level; i++) {
      update[i] = head;
    }

    current_level = node_level;
  }

  SkipNode *node = new SkipNode(key, record, node_level);

  for (int i = 0; i < node_level; i++) {
    node->forward[i] = update[i]->forward[i];

    update[i]->forward[i] = node;
  }
}

SkipList::Iterator::Iterator(SkipNode *node) : current(node) {}

bool SkipList::Iterator::Valid() const { return current != nullptr; }

void SkipList::Iterator::Next() {
  if (current)
    current = current->forward[0];
}

const Record &SkipList::Iterator::Value() const { return current->record; }

SkipList::Iterator SkipList::Begin() const {

  return Iterator(head->forward[0]);
}