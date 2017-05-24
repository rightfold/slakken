#include "thread.hpp"

#include <cstddef>

using namespace slakken;

std::size_t thread::root_count() const {
  return variables.size() + operands.size();
}

value const& thread::root_at(std::size_t index) const {
  if (index < variables.size()) {
    return *variables[index];
  } else {
    return *operands[index - variables.size()];
  }
}
