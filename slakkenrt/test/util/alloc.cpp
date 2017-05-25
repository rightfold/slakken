#include "alloc.hpp"

#include "../../src/value.hpp"

#include <cstddef>

using namespace slakken;
using namespace slakken_test;

std::size_t vector_soil::root_count() const {
  return size();
}

value const* vector_soil::root_at(std::size_t index) const {
  return at(index);
}
