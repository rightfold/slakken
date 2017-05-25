#include "../src/alloc.hpp"
#include "../src/bytecode.hpp"
#include "../src/function.hpp"
#include "../src/value.hpp"

#include <catch.hpp>

#include <cstdint>
#include <utility>

using namespace slakken;
using namespace slakken::bytecode;

TEST_CASE("decode_module", "[decode_module]") {
  function_set functions;
  alloc alloc;

  char data[] = {
    // Magic number
    0x43, 0x51,

    // Version number
    0x00, 0x00,
    0x00, 0x00,

    // Compiler
    0x0D, 0x00, 0x00, 0x00,
    's', 'l', 'a', 'k', 'k', 'e', 'n', 'c', ' ', 'v', '0', '.', '0',

    // Constant pool
    0x00, 0x00, 0x00, 0x00,

    // Function map
    0x00, 0x00, 0x00, 0x00,
  };

  SECTION("ok") {
    auto const_pool = decode_module(functions, alloc, data, sizeof(data));
    REQUIRE(functions.functions.size() == 0);
    REQUIRE(const_pool.size() == 0);
  }

  SECTION("eof") {
    for (std::size_t len = 0; len < sizeof(data); ++len) {
      REQUIRE_THROWS_AS(
        decode_module(functions, alloc, data, len),
        decode_eof_error
      );
    }
  }

  SECTION("invalid magic number") {
    std::swap(data[0], data[1]);
    REQUIRE_THROWS_AS(
      decode_module(functions, alloc, data, sizeof(data)),
      decode_magic_error
    );
  }

  SECTION("invalid version number") {
    data[4] = 0x01;
    REQUIRE_THROWS_AS(
      decode_module(functions, alloc, data, sizeof(data)),
      decode_version_error
    );
  }
}
