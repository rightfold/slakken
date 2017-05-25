#pragma once

#include "alloc.hpp"
#include "function.hpp"
#include "thread.hpp"

namespace slakken {
  /**
   * The status of a thread after resumption.
   */
  enum class resume_status {
    finished, /**< The thread has finished and may no longer be resumed. */
    breakpoint, /**< The thread has encountered a breakpoint. */
  };

  /**
   * Resume a thread.
   */
  resume_status resume(alloc&, function_set const&, thread&);
}
