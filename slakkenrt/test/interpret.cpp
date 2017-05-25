#include "../src/alloc.hpp"
#include "../src/bytecode.hpp"
#include "../src/function.hpp"
#include "../src/interpret.hpp"
#include "../src/thread.hpp"

#include <catch.hpp>

using namespace slakken;
using namespace slakken::bytecode;

#define SLAKKEN_PUSH_OP_0(function_id, opcode) \
  do { \
    instruction inst; \
    inst.what = (opcode); \
    functions.functions.at(function_id).instructions.push_back(inst); \
  } while (false)

#define SLAKKEN_PUSH_OP_1(function_id, opcode, op0_t, op0_v) \
  do { \
    instruction inst; \
    inst.what = (opcode); \
    inst.op0.op0_t = (op0_v); \
    functions.functions.at(function_id).instructions.push_back(inst); \
  } while (false)

TEST_CASE("resume", "[resume]") {
  alloc alloc;
  function_set functions;
  thread thread;

  auto main = functions.alloc("");

  SECTION("nop") {
    SLAKKEN_PUSH_OP_0(main, opcode::nop);
    SLAKKEN_PUSH_OP_0(main, opcode::brk);

    thread.program_counters.emplace_back(main, 0);
    thread.param_counts.push_back(0);

    auto status = resume(alloc, functions, thread);

    REQUIRE(status == resume_status::breakpoint);
  }

  SECTION("brk") {
    SLAKKEN_PUSH_OP_0(main, opcode::brk);

    thread.program_counters.emplace_back(main, 0);
    thread.param_counts.push_back(0);

    auto status = resume(alloc, functions, thread);

    REQUIRE(status == resume_status::breakpoint);
  }

  SECTION("ck.param.eq") {
    SECTION("succeed") {
      SECTION("0") {
        SLAKKEN_PUSH_OP_1(main, opcode::ck_param_eq, imm, 0);
        SLAKKEN_PUSH_OP_0(main, opcode::brk);

        thread.program_counters.emplace_back(main, 0);
        thread.param_counts.push_back(0);

        auto status = resume(alloc, functions, thread);

        REQUIRE(status == resume_status::breakpoint);
      }

      SECTION("1") {
        SLAKKEN_PUSH_OP_1(main, opcode::ck_param_eq, imm, 1);
        SLAKKEN_PUSH_OP_0(main, opcode::brk);

        thread.program_counters.emplace_back(main, 0);
        thread.param_counts.push_back(1);
        thread.params.push_back(&alloc.alloc_array(nullptr, 0));

        auto status = resume(alloc, functions, thread);

        REQUIRE(status == resume_status::breakpoint);
      }
    }

    SECTION("fail") {
      SECTION("1 != 0") {
        SLAKKEN_PUSH_OP_1(main, opcode::ck_param_eq, imm, 1);
        SLAKKEN_PUSH_OP_0(main, opcode::brk);

        thread.program_counters.emplace_back(main, 0);
        thread.param_counts.push_back(0);

        REQUIRE_THROWS(
          resume(alloc, functions, thread)
        );
      }

      SECTION("2 != 3") {
        SLAKKEN_PUSH_OP_1(main, opcode::ck_param_eq, imm, 2);
        SLAKKEN_PUSH_OP_0(main, opcode::brk);

        thread.program_counters.emplace_back(main, 0);
        thread.param_counts.push_back(3);
        thread.params.push_back(&alloc.alloc_array(nullptr, 0));
        thread.params.push_back(&alloc.alloc_array(nullptr, 0));
        thread.params.push_back(&alloc.alloc_array(nullptr, 0));

        REQUIRE_THROWS(
          resume(alloc, functions, thread)
        );
      }
    }
  }
}

#undef SLAKKEN_PUSH_OP_1
#undef SLAKKEN_PUSH_OP_0
