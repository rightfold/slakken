#include "bytecode.hpp"

#include <cstddef>

using namespace slakken;

std::size_t bytecode::root_count() const {
  return roots.size();
}

value const& bytecode::root_at(std::size_t index) const {
  return *roots[index];
}

bytecode decode(alloc& alloc, char const* binary, std::size_t binary_size) {
  bytecode bytecode;
  return bytecode;
}
