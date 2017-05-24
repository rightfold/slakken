#pragma once

#include <cstdlib>
#include <memory>
#include <string>

namespace slakken {
  class value {
  public:
    value() noexcept;
    value(const value&) = delete;
    value(value&&) = delete;
    value& operator=(const value&) = delete;
    value& operator=(value&&) = delete;
    virtual ~value();

    virtual std::size_t size() const;
    virtual value const& operator[](std::size_t) const;

  private:
    mutable bool marked;

    friend class alloc;
  };

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

  class atom_value : public value {
  public:
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

  class float_value : public value {
  public:
    double get() const noexcept;

  private:
    float_value() noexcept;

    double value;

    friend class alloc;
  };
}
