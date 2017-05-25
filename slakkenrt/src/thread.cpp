#include "thread.hpp"

#include <cstddef>

using namespace slakken;

program_counter::program_counter(std::size_t function_id, std::size_t offset)
  : function_id(function_id)
  , offset(offset) {
}

std::size_t thread::root_count() const {
  return variables.size() + params.size() + operands.size();
}

value const* thread::root_at(std::size_t index) const {
  if (index < variables.size()) {
    return variables[index];
  } else if (index < variables.size() + params.size()) {
    return params[index - variables.size()];
  } else {
    return operands[index - variables.size() - params.size()];
  }
}
