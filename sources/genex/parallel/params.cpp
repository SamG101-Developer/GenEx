module;
#include <genex/macros.hpp>

export module genex.parallel.params;
import std;

namespace genex::parallel {
  /**
   * Tuning knobs shared by every parallel algorithm. The defaults are deliberately conservative: parallelism is only
   * ever a win when the per-element work is expensive enough to pay for the thread spawn (roughly 10-30us each on
   * Linux), so small ranges silently run on the calling thread.
   *
   *  - @c threads is the total number of workers, including the calling thread, which always takes the first chunk.
   *    @c 0 means "ask the hardware"; @c 1 forces the serial path.
   *  - @c threshold is the element count below which the range is not split at all. Set it to @c 0 to always split
   *    (useful in benchmarks, and when the callable is known to be very expensive).
   */
  export struct params {
    std::size_t threads = 0;
    std::size_t threshold = 1uz << 14;
  };

  namespace detail {
    export GENEX_INLINE auto worker_count(const std::size_t total, const params &p) noexcept -> std::size_t {
      if (p.threads == 1 or total < 2 or total < p.threshold) { return 1; }

      auto n = p.threads;
      if (n == 0) {
        n = static_cast<std::size_t>(std::thread::hardware_concurrency());
        if (n == 0) { n = 1; }
      }
      return n < total ? n : total;
    }
  }
}
