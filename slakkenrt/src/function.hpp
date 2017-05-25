#pragma once

#include "bytecode.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace slakken {
  /**
   * A function.
   */
  struct function {
    function();

    /**
     * The number of local variables the function uses.
     */
    std::size_t variable_count;

    /**
     * The function body.
     */
    std::vector<bytecode::instruction> instructions;
  };

  /**
   * Thrown when an attempt was made to redefine a function.
   */
  class function_realloc : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  /**
   * A set of functions.
   */
  struct function_set {
    /**
     * Allocate a new function with zero local variables and an empty body,
     * ready for mutation.
     *
     * \exception function_realloc if a function with this name already exists.
     */
    std::size_t alloc(std::string name);

    /**
     * All functions. The index of the function is the function identifier.
     */
    std::vector<function> functions;

    /**
     * A mapping from function names to function identifiers.
     */
    std::unordered_map<std::string, std::size_t> function_names;
  };
}
