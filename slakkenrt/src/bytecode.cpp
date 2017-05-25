#include "bytecode.hpp"
#include "function.hpp"
#include "value.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

using namespace slakken;
using namespace slakken::bytecode;

std::size_t const_pool::root_count() const {
  return size();
}

value const* const_pool::root_at(std::size_t index) const {
  return operator[](index);
}

decode_eof_error::decode_eof_error()
  : decode_error("unexpected eof") {
}

decode_magic_error::decode_magic_error()
  : decode_error("invalid magic number") {
}

decode_version_error::decode_version_error()
  : decode_error("invalid version number") {
}

decode_const_type_error::decode_const_type_error()
  : decode_error("invalid constant type") {
}

decode_const_range_error::decode_const_range_error()
  : decode_error("constant index out of range") {
}

decode_function_definition_site_error::decode_function_definition_site_error()
  : decode_error("invalid function definition site") {
}

decode_function_range_error::decode_function_range_error()
  : decode_error("function index out of range") {
}

decode_opcode_error::decode_opcode_error()
  : decode_error("invalid opcode") {
}

namespace {
  template<typename T>
  T decode(char const*& begin, char const* end) {
    if (sizeof(T) > static_cast<std::size_t>(end - begin)) {
      throw decode_eof_error();
    }
    T result;
    std::memcpy(&result, begin, sizeof(T));
    begin += sizeof(T);
    return result;
  }

  void decode_array_constant(const_pool& consts, alloc& alloc, char const*& begin, char const* end) {
    auto length = decode<std::uint32_t>(begin, end);
    std::vector<value const*> elements(length);
    std::generate(elements.begin(), elements.end(), [&] {
      auto element_index = decode<std::uint32_t>(begin, end);
      if (element_index >= consts.size()) {
        throw decode_const_range_error();
      }
      return consts[element_index];
    });
    consts.push_back(&alloc.alloc_array(elements.data(), elements.size()));
  }

  void decode_atom_constant(const_pool& consts, alloc& alloc, char const*& begin, char const* end) {
    auto length = decode<std::uint32_t>(begin, end);
    if (length > end - begin) {
      throw decode_eof_error();
    }
    consts.push_back(&alloc.alloc_atom(begin, length));
    begin += length;
  }

  void decode_float_constant(const_pool& consts, alloc& alloc, char const*& begin, char const* end) {
    static_assert(sizeof(double) == 8, "sizeof(double) must be 8");
    auto value = decode<double>(begin, end);
    consts.push_back(&alloc.alloc_float(value));
  }
}

const_pool slakken::bytecode::decode_module(function_set& functions, alloc& alloc, char const* binary, std::size_t binary_size) {
  auto begin = binary, end = binary + binary_size;

  auto magic = decode<std::uint16_t>(begin, end);
  if (magic != 0x5143) {
    throw decode_magic_error();
  }

  auto major_version = decode<std::uint16_t>(begin, end);
  if (major_version != 0) {
    throw decode_version_error();
  }

  auto minor_version = decode<std::uint16_t>(begin, end);
  if (minor_version != 0) {
    throw decode_version_error();
  }

  auto compiler_length = decode<std::uint32_t>(begin, end);
  if (compiler_length > end - begin) {
    throw decode_eof_error();
  }
  begin += compiler_length;

  auto const_pool_length = decode<std::uint32_t>(begin, end);
  if (const_pool_length > end - begin) {
    throw decode_eof_error();
  }
  auto const_pool = decode_const_pool(alloc, begin, const_pool_length);
  begin += const_pool_length;

  auto function_map_length = decode<std::uint32_t>(begin, end);
  if (function_map_length > end - begin) {
    throw decode_eof_error();
  }
  auto function_map = decode_function_map(functions, begin, function_map_length);
  begin += function_map_length;

  while (begin != end) {
    auto function_index = decode<std::uint32_t>(begin, end);

    if (function_index >= function_map.size()) {
      throw decode_function_range_error();
    }
    auto function_id = function_map[function_index];

    auto& function = functions.functions[function_id];
    function.variable_count = decode<std::uint32_t>(begin, end);

    auto body_length = decode<std::uint32_t>(begin, end);
    if (body_length > end - begin) {
      throw decode_eof_error();
    }
    function.instructions = decode_instructions(function_map, const_pool, begin, body_length);
    begin += body_length;
  }

  return const_pool;
}

const_pool slakken::bytecode::decode_const_pool(alloc& alloc, char const* binary, std::size_t binary_size) {
  const_pool consts;
  for (auto begin = binary, end = binary + binary_size; begin != end;) {
    switch (*begin++) {
    case 0x00:
      decode_array_constant(consts, alloc, begin, end);
      break;
    case 0x01:
      decode_atom_constant(consts, alloc, begin, end);
      break;
    case 0x02:
      decode_float_constant(consts, alloc, begin, end);
      break;
    default:
      throw decode_const_type_error();
    }
  }
  return consts;
}

function_map slakken::bytecode::decode_function_map(function_set& functions, char const* binary, std::size_t binary_size) {
  function_map funcs;
  for (auto begin = binary, end = binary + binary_size; begin != end;) {
    auto definition_site = *begin++;

    auto name_length = decode<std::uint32_t>(begin, end);
    if (name_length > end - begin) {
      throw decode_eof_error();
    }
    std::string name(begin, begin + name_length);
    begin += name_length;

    std::size_t function_id;
    switch (definition_site) {
    case 0x00:
      function_id = functions.function_names.at(name);
      break;
    case 0x01:
      function_id = functions.alloc(std::move(name));
      break;
    default:
      throw decode_function_definition_site_error();
    }
    funcs.push_back(function_id);
  }
  return funcs;
}

std::vector<instruction> slakken::bytecode::decode_instructions(function_map const& functions, const_pool const& consts, char const* binary, std::size_t binary_size) {
  std::vector<instruction> insts;
  for (auto begin = binary, end = binary + binary_size; begin != end;) {
#include "../build/decode.inc"
    throw decode_opcode_error();
  }
  return insts;
}
