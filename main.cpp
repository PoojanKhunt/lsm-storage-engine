#include "include/db.h"
#include <iostream>

int main() {
  DB db;

  auto val = db.Get("key15x");

  if (val)
    std::cout << *val << '\n';
  else
    std::cout << "not found\n";
}