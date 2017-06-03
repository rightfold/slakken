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
    throw function_realloc(std::move(name));
  }
  functions.emplace_back();
  auto id = functions.size() - 1;
  function_names.emplace(std::move(name), id);
  return id;
}

function& function_set::operator[](std::size_t index) noexcept {
  return functions[index];
}

function const& function_set::operator[](std::size_t index) const noexcept {
  return functions[index];
}

std::size_t function_set::operator[](std::string const& name) const {
  if (!function_names.count(name)) {
    throw function_unknown(name);
  }
  return function_names.at(name);
}

std::size_t function_set::size() const noexcept {
  return functions.size();
}
