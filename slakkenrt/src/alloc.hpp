#pragma once

#include "value.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace slakken {
  class alloc {
  public:
    value const& alloc_array(value const* const*, std::size_t);
    value const& alloc_atom(char const*, std::size_t);
    value const& alloc_float(double);

  private:
    struct value_delete {
      void operator()(value const*) const;
    };

    template<typename T>
    using owning = std::unique_ptr<T, value_delete>;

    value const& register_(owning<value const>);

    std::vector<owning<value const>> values;
  };
}
