#pragma once

#include "alloc.hpp"
#include "value.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace slakken {
  struct function_set;

  /**
   * Bytecode types and functions.
   */
  namespace bytecode {
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
      operand op0;
      operand op1;
    };

    /**
     * Constants from a bytecode file.
     */
    struct const_pool : std::vector<value const*>, soil {
      std::size_t root_count() const override;
      value const* root_at(std::size_t) const override;
    };

    /**
     * A mapping from function indices to function identifiers.
     */
    using function_map = std::vector<std::size_t>;

    /**
     * An error that occurs during bytecode decoding.
     */
    class decode_error : public std::runtime_error {
    public:
      using std::runtime_error::runtime_error;
    };

    /**
     * An error that occurs because there was too little data when decoding
     * bytecode.
     */
    class decode_eof_error : public decode_error {
    public:
      decode_eof_error();
    };

    /**
     * An error that occurs because an invalid magic number was encountered.
     */
    class decode_magic_error : public decode_error {
    public:
      decode_magic_error();
    };

    /**
     * An error that occurs because an invalid version number was encountered.
     */
    class decode_version_error : public decode_error {
    public:
      decode_version_error();
    };

    /**
     * An error that occurs because a constant pool entry had an invalid type.
     */
    class decode_const_type_error : public decode_error {
    public:
      decode_const_type_error();
    };

    /**
     * An error that occurs because a constant pool lookup was out of range.
     */
    class decode_const_range_error : public decode_error {
    public:
      decode_const_range_error();
    };

    /**
     * An error that occurs because a function definition site was invalid.
     */
    class decode_function_definition_site_error : public decode_error {
    public:
      decode_function_definition_site_error();
    };

    /**
     * An error that occurs because a function map lookup was out of range.
     */
    class decode_function_range_error : public decode_error {
    public:
      decode_function_range_error();
    };

    /**
     * An error that occurs because an instruction had an invalid opcode.
     */
    class decode_opcode_error : public decode_error {
    public:
      decode_opcode_error();
    };

    /**
     * Decode a bytecode module file.
     *
     * \exception decode_error on invalid input.
     * \exception function_realloc if a function was redefined.
     */
    const_pool decode_module(function_set&, alloc&, char const*, std::size_t);

    /**
     * Decode a bytecode constant pool.
     *
     * \relate const_pool
     *
     * \exception decode_error on invalid input.
     */
    const_pool decode_const_pool(alloc&, char const*, std::size_t);

    /**
     * Decode a bytecode function map.
     *
     * \relate function_map
     *
     * \exception decode_error on invalid input.
     * \exception function_realloc if a function was redefined.
     */
    function_map decode_function_map(function_set&, char const*, std::size_t);

    /**
     * Decode bytecode instructions.
     *
     * \exception decode_error on invalid input.
     */
    std::vector<instruction> decode_instructions(function_map const&, const_pool const&, char const*, std::size_t);
  }
}
