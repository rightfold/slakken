#pragma once

#include "alloc.hpp"
#include "function.hpp"
#include "value.hpp"

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
  class thread : public soil {
  public:
    std::size_t root_count() const override;
    value const* root_at(std::size_t) const override;

    /**
     * Return whether there are any stack frames.
     */
    bool empty() const noexcept;

    /**
     * Return the program counter on the top of the call stack. The behavior is
     * undefined if the call stack is empty.
     */
    struct program_counter& program_counter() noexcept;

    /**
     * The number of parameters on the current stack frame. The behavior is
     * undefined if the call stack is empty.
     */
    std::size_t param_count() const noexcept;

    /**
     * The parameter at the given index. The behavior is undefined if this
     * index is out of bounds or the call stack is empty.
     */
    value const& param(std::size_t) const noexcept;

    /**
     * Push a new stack frame for the given function with the given number of
     * parameters.
     */
    void call(std::size_t, function const&, std::size_t);

    /**
     * Pop a stack frame off the call stack. The behavior is undefined if the
     * given function is not the one that was passed to the corresponding
     * \c call call or if the call stack is empty.
     */
    void return_(function const&);

    /**
     * Pop an operand off the operand stack. The behavior is undefined if the
     * operand stack is empty.
     */
    template<typename T = value>
    T const& pop();

    /**
     * Push an operand onto the operand stack.
     */
    void push(value const&);

  private:
    std::vector<struct program_counter> program_counters;
    std::vector<value const*> variables;
    std::vector<std::size_t> param_counts;
    std::vector<value const*> params;
    std::vector<value const*> operands;
  };
}

namespace slakken {
  template<typename T>
  T const& thread::pop() {
    auto& value = *operands.back();
    operands.pop_back();
    return dynamic_cast<T const&>(value);
  }
}
