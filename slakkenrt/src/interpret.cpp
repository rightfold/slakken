#include "alloc.hpp"
#include "bytecode.hpp"
#include "function.hpp"
#include "interpret.hpp"
#include "thread.hpp"

#include <cstdint>

using namespace slakken;
using namespace slakken::bytecode;

namespace {
  std::int32_t pop_int(thread& thread) {
    auto& value = thread.pop<float_value>();
    return value.get();
  }

  void push_int(alloc& alloc, thread& thread, std::int32_t value) {
    thread.push(alloc.alloc_float(value));
  }
}

resume_status slakken::resume(alloc& alloc, function_set const& functions, thread& thread) {
  while (!thread.empty()) {
    auto& pc = thread.program_counter();
    auto& fn = functions[pc.function_id];
    auto& in = fn.instructions[pc.offset];
    switch (in.what) {
      case opcode::nop:
        ++pc.offset;
        break;

      case opcode::brk:
        return resume_status::breakpoint;

      case opcode::ck_param_eq: {
        auto actual = thread.param_count();
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
        auto& value = thread.param(in.op0.imm);
        thread.push(value);
        ++pc.offset;
        break;
      };

      case opcode::ld_const:
        thread.push(*in.op0.val);
        ++pc.offset;
        break;

      case opcode::alloc_array: {
        std::vector<value const*> values;
        values.resize(in.op0.imm);
        for (auto i = in.op0.imm; i > 0; --i) {
          auto& value = thread.pop();
          values[i - 1] = &value;
        }
        auto& array = alloc.alloc_array(values.data(), values.size());
        thread.push(array);
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
        thread.return_(fn);
        break;

      case opcode::call_direct:
        ++pc.offset;
        thread.call(in.op0.fun, functions[in.op0.fun], in.op1.imm);
        break;
    }
  }
  return resume_status::finished;
}
