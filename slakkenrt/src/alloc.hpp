#pragma once

#include "value.hpp"

#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>

namespace slakken {
  class soil {
  public:
    virtual ~soil();

    virtual std::size_t root_count() const = 0;
    virtual value const& root_at(std::size_t) const = 0;
  };

  class alloc {
  public:
    value const& alloc_array(value const* const*, std::size_t);
    value const& alloc_atom(char const*, std::size_t);
    value const& alloc_float(double);

    void add_soil(soil const&);
    void remove_soil(soil const&);

    void collect_garbage();

  private:
    struct value_delete {
      void operator()(value const*) const;
    };

    template<typename T>
    using owning = std::unique_ptr<T, value_delete>;

    value const& register_(owning<value const>);

    std::unordered_map<soil const*, std::size_t> soils;
    std::vector<owning<value const>> values;

    void mark_roots();
    void mark_soil(soil const&);
    static void mark_children(value const&);
  };
}
