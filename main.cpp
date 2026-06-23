#include "include/db.h"
#include <iostream>

int main() {
  DB db;

  auto v = db.Get("key45");

  if (v)
    std::cout << *v << '\n';
  else
    std::cout << "not found\n";
}