#include "../src/alloc.hpp"
#include "../src/bytecode.hpp"
#include "../src/value.hpp"

#include <catch.hpp>

#include <cstdint>

using namespace slakken;

TEST_CASE("decode_const_pool", "[decode_const_pool]") {
  const_pool consts;
  alloc alloc;

  SECTION("empty") {
    decode_const_pool(consts, alloc, nullptr, 0);
    REQUIRE(consts.size() == 0);
  }

  SECTION("non-empty") {
    char data[] = {
      0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x03, 0x00, 0x00, 0x00, 'A', 'B', 'C',
      0x02, -102, -103, -103, -103, -103, -103, -71, 63,
      0x00, 0x03, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00,
    };

    decode_const_pool(consts, alloc, data, sizeof(data));

    REQUIRE(consts.size() == 4);

    auto& const0 = dynamic_cast<array_value const&>(*consts.at(0));
    auto& const1 = dynamic_cast<atom_value const&>(*consts.at(1));
    auto& const2 = dynamic_cast<float_value const&>(*consts.at(2));
    auto& const3 = dynamic_cast<array_value const&>(*consts.at(3));

    REQUIRE(const0.size() == 0);
    REQUIRE(const1.get_string() == "ABC");
    REQUIRE(const2.get() == 0.1);
    REQUIRE(const3.size() == 3);
    REQUIRE(&const3[0] == &const0);
    REQUIRE(&const3[1] == &const1);
    REQUIRE(&const3[2] == &const2);
  }

  SECTION("eof") {
    char data[] = {
      0x00, 0x00, 0x00, 0x00,
    };
    REQUIRE_THROWS_AS(
      decode_const_pool(consts, alloc, data, sizeof(data)),
      decode_eof_error
    );
  }

  SECTION("invalid constant type") {
    char data[] = {
      0x03, 0x00, 0x00, 0x00, 0x00,
    };
    REQUIRE_THROWS_AS(
      decode_const_pool(consts, alloc, data, sizeof(data)),
      decode_const_type_error
    );
  }

  SECTION("invalid constant index") {
    char data[] = {
      0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
    };
    REQUIRE_THROWS_AS(
      decode_const_pool(consts, alloc, data, sizeof(data)),
      decode_const_range_error
    );
  }
}
