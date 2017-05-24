#pragma once

#include "alloc.hpp"
#include "value.hpp"

#include <cstddef>
#include <cstdint>

namespace slakken {
  /**
   * An opcode is like a function.
   */
  enum class opcode : uint8_t {
#include "../build/opcode.inc"
  };

  /**
   * An operand is like an argument.
   */
  union operand {
    uint32_t imm;
    std::size_t fun;
    value const* val;
  };

  /**
   * An instruction is like a function call.
   */
  struct instruction {
    opcode what;
    operand op1;
    operand op2;
  };

  /**
   * A sequence of instructions.
   */
  struct bytecode : soil {
    std::size_t root_count() const override;
    value const& root_at(std::size_t) const override;

    /**
     * All the value operands.
     */
    std::vector<value const*> roots;

    /**
     * The instruction to be executed, starting at the first one.
     */
    std::vector<instruction> instructions;
  };

  /**
   * Decode bytecode.
   */
  bytecode decode(alloc&, char const*, std::size_t);
}
