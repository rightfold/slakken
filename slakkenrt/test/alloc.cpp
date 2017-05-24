#include "util/alloc.hpp"

#include "../src/alloc.hpp"
#include "../src/value.hpp"

#include <catch.hpp>

using namespace slakken;
using namespace slakken_test;

TEST_CASE("alloc_*", "[alloc]") {
  alloc alloc;

  SECTION("array") {
    SECTION("empty") {
      auto& value_base = alloc.alloc_array(nullptr, 0);
      auto& value = dynamic_cast<array_value const&>(value_base);
      REQUIRE(value.size() == 0);
    }

    SECTION("singleton") {
      auto& empty = alloc.alloc_array(nullptr, 0);
      value const* elements[] = { &empty };
      auto& value_base = alloc.alloc_array(elements, 1);
      auto& value = dynamic_cast<array_value const&>(value_base);
      REQUIRE(value.size() == 1);
      REQUIRE(&value[0] == elements[0]);
    }
  }

  SECTION("atom") {
    auto& value_base = alloc.alloc_atom("foo", 3);
    auto& value = dynamic_cast<atom_value const&>(value_base);
    REQUIRE(value.get_string() == "foo");
  }

  SECTION("float") {
    auto& value_base = alloc.alloc_float(3.14);
    auto& value = dynamic_cast<float_value const&>(value_base);
    REQUIRE(value.get() == 3.14);
  }
}

TEST_CASE("collect_garbage", "[alloc]") {
  alloc alloc;

  SECTION("no roots") {
    alloc.alloc_array(nullptr, 0);
    alloc.alloc_atom("foo", 3);
    alloc.alloc_float(3.14);

    alloc.collect_garbage();

    REQUIRE(alloc.stat_live() == 0);
  }

  SECTION("roots") {
    vector_soil soil;
    alloc.add_soil(soil);

    auto& value1 = alloc.alloc_array(nullptr, 0);
    alloc.alloc_atom("foo", 3);
    alloc.alloc_float(3.14);

    soil.push_back(&value1);

    alloc.collect_garbage();

    REQUIRE(alloc.stat_live() == 1);
  }

  SECTION("roots with references") {
    vector_soil soil;
    alloc.add_soil(soil);

    auto& value1 = alloc.alloc_atom("foo", 3);
    alloc.alloc_float(3.14);
    value const* elements[] = { &value1 };
    auto& value2 = alloc.alloc_array(elements, 1);

    soil.push_back(&value2);

    alloc.collect_garbage();

    REQUIRE(alloc.stat_live() == 2);
  }
}
