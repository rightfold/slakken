#include "value.hpp"

#include <cassert>

using namespace slakken;

value::value() noexcept
  : marked(false) {
}

value::~value() = default;

std::size_t value::size() const {
  return 0;
}

value const& value::operator[](std::size_t) const {
  assert(false);
}

std::size_t array_value::size() const {
  return size_field;
}

value const& array_value::operator[](std::size_t index) const {
  assert(index < size_field);
  return *elements[index];
}

array_value::array_value() noexcept = default;

std::string atom_value::get_string() const {
  return std::string(name, name + size_field);
}

atom_value::atom_value() noexcept = default;

double float_value::get() const noexcept {
  return value;
}

float_value::float_value() noexcept = default;
