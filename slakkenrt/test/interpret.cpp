#include "../src/alloc.hpp"
#include "../src/bytecode.hpp"
#include "../src/function.hpp"
#include "../src/interpret.hpp"
#include "../src/thread.hpp"

#include <catch.hpp>

#include <cmath>

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
    auto test_ck_param_eq = [&] (std::size_t n, std::size_t m) {
      SLAKKEN_PUSH_OP_1(main, opcode::ck_param_eq, imm, n);
      SLAKKEN_PUSH_OP_0(main, opcode::brk);

      thread.program_counters.emplace_back(main, 0);
      thread.param_counts.push_back(m);
      for (decltype(m) i = 0; i < m; ++i) {
        auto& param = alloc.alloc_array(nullptr, 0);
        thread.params.push_back(&param);
      }

      if (n == m) {
        auto status = resume(alloc, functions, thread);
        REQUIRE(status == resume_status::breakpoint);
      } else {
        REQUIRE_THROWS(resume(alloc, functions, thread));
      }
    };

    SECTION("succeed") {
      SECTION("0") { test_ck_param_eq(0, 0); }
      SECTION("1") { test_ck_param_eq(1, 1); }
      SECTION("2") { test_ck_param_eq(2, 2); }
      SECTION("3") { test_ck_param_eq(3, 3); }
      SECTION("4") { test_ck_param_eq(4, 4); }
    }

    SECTION("fail") {
      SECTION("0 != 1") { test_ck_param_eq(0, 1); }
      SECTION("1 != 2") { test_ck_param_eq(1, 2); }
      SECTION("2 != 3") { test_ck_param_eq(2, 3); }
      SECTION("3 != 2") { test_ck_param_eq(3, 2); }
      SECTION("2 != 1") { test_ck_param_eq(2, 1); }
      SECTION("1 != 0") { test_ck_param_eq(1, 0); }
    }
  }

  SECTION("neg.int") {
    auto test_neg_int = [&] (double in, double out) {
      SLAKKEN_PUSH_OP_0(main, opcode::neg_int);
      SLAKKEN_PUSH_OP_0(main, opcode::brk);

      thread.program_counters.emplace_back(main, 0);
      thread.param_counts.push_back(0);
      thread.operands.push_back(&alloc.alloc_float(in));

      auto status = resume(alloc, functions, thread);

      REQUIRE(status == resume_status::breakpoint);

      auto& result = *thread.operands.at(0);
      REQUIRE(dynamic_cast<float_value const&>(result).get() == out);
      if (in == 0.0) {
        REQUIRE(!std::signbit(dynamic_cast<float_value const&>(result).get()));
      }
    };

    SECTION("42") { test_neg_int(42.0, -42.0); }
    SECTION("-42") { test_neg_int(-42.0, 42.0); }
    SECTION("0") { test_neg_int(0.0, 0.0); }
    SECTION("2147483647") { test_neg_int(2147483647.0, -2147483647.0); }
    SECTION("-2147483647") { test_neg_int(-2147483647.0, 2147483647.0); }
    SECTION("-2147483648") { test_neg_int(-2147483648.0, -2147483648.0); }
  }
}

#undef SLAKKEN_PUSH_OP_1
#undef SLAKKEN_PUSH_OP_0
