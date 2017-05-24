#pragma once

#include "../../src/alloc.hpp"
#include "../../src/value.hpp"

#include <cstddef>
#include <vector>

namespace slakken_test {
  struct vector_soil : std::vector<slakken::value const*>, slakken::soil {
    std::size_t root_count() const override;
    slakken::value const& root_at(std::size_t) const override;
  };
}
