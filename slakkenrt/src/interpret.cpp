#include "alloc.hpp"
#include "bytecode.hpp"
#include "function.hpp"
#include "interpret.hpp"
#include "thread.hpp"

#include <cstdint>

using namespace slakken;
using namespace slakken::bytecode;

namespace {
  template<typename T>
  void shrink_by(std::vector<T>& vector, std::size_t by) {
    vector.resize(vector.size() - by);
  }

  template<typename T>
  void grow_by(std::vector<T>& vector, std::size_t by) {
    vector.resize(vector.size() + by);
  }

  template<typename T = value>
  T const& pop(thread& thread) {
    auto& value = *thread.operands.back();
    thread.operands.pop_back();
    return dynamic_cast<T const&>(value);
  }

  std::int32_t pop_int(thread& thread) {
    auto& value = pop<float_value>(thread);
    return value.get();
  }

  void push(thread& thread, value const& value) {
    thread.operands.push_back(&value);
  }

  void push_int(alloc& alloc, thread& thread, std::int32_t value) {
    push(thread, alloc.alloc_float(value));
  }
}

resume_status slakken::resume(alloc& alloc, function_set const& functions, thread& thread) {
  while (!thread.program_counters.empty()) {
    auto& pc = thread.program_counters.back();
    auto& fn = functions[pc.function_id];
    auto& in = fn.instructions[pc.offset];
    switch (in.what) {
      case opcode::nop:
        ++pc.offset;
        break;

      case opcode::brk:
        return resume_status::breakpoint;

      case opcode::ck_param_eq: {
        auto actual = thread.param_counts.back();
        if (actual != in.op0.imm) {
          throw ""; // FIXME: throw Slakken exception
        }
        ++pc.offset;
        break;
      }

      case opcode::neg_int: {
        auto value = pop_int(thread);
        push_int(alloc, thread, -value);
        ++pc.offset;
        break;
      }

      case opcode::add_int: {
        auto value1 = pop_int(thread);
        auto value2 = pop_int(thread);
        push_int(alloc, thread, value2 + value1);
        ++pc.offset;
        break;
      }

      case opcode::mul_int: {
        auto value1 = pop_int(thread);
        auto value2 = pop_int(thread);
        push_int(alloc, thread, value2 * value1);
        ++pc.offset;
        break;
      }

      case opcode::ld_param_direct: {
        auto index = thread.params.size() - 1 - in.op0.imm;
        auto& value = *thread.params[index];
        push(thread, value);
        ++pc.offset;
        break;
      };

      case opcode::ld_const:
        push(thread, *in.op0.val);
        ++pc.offset;
        break;

      case opcode::alloc_array: {
        auto index = thread.operands.size() - in.op0.imm;
        auto begin = thread.operands.data() + index;
        auto& array = alloc.alloc_array(begin, in.op0.imm);
        shrink_by(thread.operands, in.op0.imm);
        push(thread, array);
        ++pc.offset;
        break;
      };

      case opcode::br_lt_int: {
        auto value1 = pop_int(thread);
        auto value2 = pop_int(thread);
        if (value2 < value1) {
          pc.offset = in.op0.imm;
        } else {
          ++pc.offset;
        }
        break;
      };

      case opcode::ret:
        thread.program_counters.pop_back();
        shrink_by(thread.variables, fn.variable_count);
        shrink_by(thread.params, thread.param_counts.back());
        thread.param_counts.pop_back();
        break;

      case opcode::call_direct: {
        ++pc.offset;
        thread.program_counters.emplace_back(in.op0.fun, 0);

        auto& new_fn = functions[in.op0.fun];
        grow_by(thread.variables, new_fn.variable_count);

        for (decltype(in.op0.imm) i = 0; i < in.op0.imm; ++i) {
          auto& param = pop(thread);
          thread.params.push_back(&param);
        }
        thread.param_counts.push_back(in.op0.imm);

        break;
      }
    }
  }
  return resume_status::finished;
}
