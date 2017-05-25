#include "function.hpp"

#include <cstddef>
#include <string>

using namespace slakken;

function::function()
  : variable_count(0)
  , instructions() {
}

std::size_t function_set::alloc(std::string name) {
  if (function_names.count(name)) {
    throw function_realloc(name);
  }
  functions.emplace_back();
  auto id = functions.size() - 1;
  function_names.emplace(std::move(name), id);
  return id;
}
