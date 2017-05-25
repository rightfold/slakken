#pragma once

#include "value.hpp"

#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>

namespace slakken {
  /**
   * A collection of root references.
   */
  class soil {
  public:
    virtual ~soil();

    /**
     * Report the number of root references in this soil.
     */
    virtual std::size_t root_count() const = 0;

    /**
     * Report the root reference at the specified index, which must be in
     * bounds. Null pointer return values are allowed and they will be ignored.
     */
    virtual value const* root_at(std::size_t) const = 0;
  };

  /**
   * An allocator and garbage collector for values.
   */
  class alloc {
  public:
    /**
     * Create a new array with values. The first argument must be a pointer to
     * an array of non-null pointers to values. The length of the array pointed
     * to by the first argument must be at least the second argument.
     */
    value const& alloc_array(value const* const*, std::size_t);

    /**
     * Create a new atom with a name. The name must be of a length that is at
     * least the second argument. Null characters are not ignored.
     */
    value const& alloc_atom(char const*, std::size_t);

    /**
     * Create a new float with a value. The value may be NaN or an infinity.
     */
    value const& alloc_float(double);

    /**
     * Add a soil to this allocator. If the soil is already known to this
     * allocator, its reference count will be incremented.
     */
    void add_soil(soil const&);

    /**
     * Remove a soil from this allocator. If the soil was added multiple times
     * to this allocator, it must be removed an equal number of times before
     * being completely removed.
     */
    void remove_soil(soil const&);

    /**
     * Delete values that are not reachable through any known soil. Any
     * existing references to those values will be invalidated.
     */
    void collect_garbage();

    /**
     * Return the number of live values. This includes unreachable values that
     * are not yet garbage collected.
     */
    std::size_t stat_live() const;

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
