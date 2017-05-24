#pragma once

#include <cstdlib>
#include <memory>
#include <string>

namespace slakken {
  /**
   * A value.
   */
  class value {
  public:
    value() noexcept;
    value(const value&) = delete;
    value(value&&) = delete;
    value& operator=(const value&) = delete;
    value& operator=(value&&) = delete;
    virtual ~value();

    /**
     * Report how many references this value contains.
     */
    virtual std::size_t size() const;

    /**
     * Report the reference at the specified index, which must be in bounds.
     */
    virtual value const& operator[](std::size_t) const;

  private:
    mutable bool marked;

    friend class alloc;
  };

  /**
   * An array value.
   */
  class array_value : public value {
  public:
    std::size_t size() const override;
    value const& operator[](std::size_t) const override;

  private:
    array_value() noexcept;

    std::size_t size_field;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    value const* elements[];
#pragma GCC diagnostic pop

    friend class alloc;
  };

  /**
   * An atom value.
   */
  class atom_value : public value {
  public:
    /**
     * Report the name of the atom.
     */
    std::string get_string() const;

  private:
    atom_value() noexcept;

    std::size_t size_field;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    char name[];
#pragma GCC diagnostic pop

    friend class alloc;
  };

  /**
   * A float value.
   */
  class float_value : public value {
  public:
    /**
     * Report the value of the value.
     */
    double get() const noexcept;

  private:
    float_value() noexcept;

    double value;

    friend class alloc;
  };
}
