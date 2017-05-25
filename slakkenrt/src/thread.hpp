#pragma once

#include "alloc.hpp"

#include <vector>

namespace slakken {
  /**
   * A pointer to the instruction to be executed next.
   */
  struct program_counter {
    program_counter(std::size_t, std::size_t);

    /**
     * The identifier of the function that houses the bytecode this program
     * counter points into.
     */
    std::size_t function_id;

    /**
     * The offset of the next instruction within the bytecode.
     */
    std::size_t offset;
  };

  /**
   * The state of a thread.
   */
  struct thread : soil {
    std::size_t root_count() const override;
    value const* root_at(std::size_t) const override;

    /**
     * The stack of program counters.
     */
    std::vector<program_counter> program_counters;

    /**
     * The stack of collections of local variables. Within a collection of
     * local variables, the variables are stored in reverse order.
     */
    std::vector<value const*> variables;

    /**
     * The stack of parameter counts.
     */
    std::vector<std::size_t> param_counts;

    /**
     * The stack of collections of parameters. Within a collection of
     * parameters, the parameters are stored in reverse order.
     */
    std::vector<value const*> params;

    /**
     * The stack of operands used by stack manipulation instructions.
     */
    std::vector<value const*> operands;
  };
}
