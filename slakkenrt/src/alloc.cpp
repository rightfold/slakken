#include "alloc.hpp"
#include "value.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

using namespace slakken;

value const& alloc::alloc_array(value const* const* elements, std::size_t size) {
  static_assert(noexcept(array_value()), "array_value must have a noexcept constructor");

  auto memory = operator new(sizeof(array_value) + sizeof(value const*) * size);
  new (memory) array_value();
  owning<array_value> upointer(static_cast<array_value*>(memory));

  upointer->size_field = size;
  std::copy(elements, elements + size, upointer->elements);

  return register_(std::move(upointer));
}

value const& alloc::alloc_atom(char const* name, std::size_t size) {
  static_assert(noexcept(atom_value()), "atom_value must have a noexcept constructor");

  auto memory = operator new(sizeof(atom_value) + size);
  new (memory) atom_value();
  owning<atom_value> upointer(static_cast<atom_value*>(memory));

  upointer->size_field = size;
  std::copy(name, name + size, upointer->name);

  return register_(std::move(upointer));
}

value const& alloc::alloc_float(double value) {
  static_assert(noexcept(float_value()), "float_value must have a noexcept constructor");

  auto memory = operator new(sizeof(float_value));
  new (memory) float_value();
  owning<float_value> upointer(static_cast<float_value*>(memory));

  upointer->value = value;

  return register_(std::move(upointer));
}

void alloc::value_delete::operator()(value const* pointer) const {
  pointer->~value();
  operator delete(const_cast<value*>(pointer));
}

value const& alloc::register_(owning<value const> upointer) {
  auto pointer = upointer.get();
  values.push_back(std::move(upointer));
  return *pointer;
}
