#include "function.hpp"
#include "thread.hpp"
#include "value.hpp"

#include <cstddef>

using namespace slakken;

namespace {
  template<typename T>
  void grow_by(std::vector<T>& vector, std::size_t by) {
    vector.resize(vector.size() + by);
  }

  template<typename T>
  void shrink_by(std::vector<T>& vector, std::size_t by) {
    vector.resize(vector.size() - by);
  }
}

program_counter::program_counter(std::size_t function_id, std::size_t offset)
  : function_id(function_id)
  , offset(offset) {
}

std::size_t thread::root_count() const {
  return variables.size() + params.size() + operands.size();
}

value const* thread::root_at(std::size_t index) const {
  if (index < variables.size()) {
    return variables[index];
  } else if (index < variables.size() + params.size()) {
    return params[index - variables.size()];
  } else {
    return operands[index - variables.size() - params.size()];
  }
}

bool thread::empty() const noexcept {
  return program_counters.empty();
}

program_counter& thread::program_counter() noexcept {
  return program_counters.back();
}

std::size_t thread::param_count() const noexcept {
  return param_counts.back();
}

value const& thread::param(std::size_t index) const noexcept {
  return *params[params.size() - 1 - index];
}

void thread::call(std::size_t function_id, function const& function, std::size_t param_count) {
  program_counters.emplace_back(function_id, 0);

  grow_by(variables, function.variable_count);

  for (decltype(param_count) i = 0; i < param_count; ++i) {
    auto& param = pop();
    params.push_back(&param);
  }
  param_counts.push_back(param_count);
}

void thread::return_(function const& function) {
  program_counters.pop_back();
  shrink_by(variables, function.variable_count);
  shrink_by(params, param_counts.back());
  param_counts.pop_back();
}

void thread::push(value const& value) {
  operands.push_back(&value);
}
