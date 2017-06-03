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
   * Thrown when an attempt was made to look up an unknown function.
   */
  class function_unknown : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  /**
   * A set of functions.
   */
  class function_set {
  public:
    /**
     * Allocate a new function with zero local variables and an empty body,
     * ready for mutation.
     *
     * \exception function_realloc if a function with this name already exists.
     */
    std::size_t alloc(std::string name);

    /**
     * Look up a function by its identifier. If the identifier is out of bounds
     * the behavior is undefined.
     */
    function& operator[](std::size_t) noexcept;

    /**
     * Look up a function by its identifier. If the identifier is out of bounds
     * the behavior is undefined.
     */
    function const& operator[](std::size_t) const noexcept;

    /**
     * Look up a function identifier by the name of the function.
     *
     * \exception function_unknown if the function is not known.
     */
    std::size_t operator[](std::string const&) const;

    /**
     * How many functions are defined?
     */
    std::size_t size() const noexcept;

  private:
    std::vector<function> functions;
    std::unordered_map<std::string, std::size_t> function_names;
  };
}
