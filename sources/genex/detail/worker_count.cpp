module;
#include <genex/detail/macros.hpp>

export module genex.detail.worker_count;
import genex.parallel.params;
import std;

namespace genex::parallel::detail {
  export GENEX_INLINE auto worker_count(
    const std::size_t total, const params &p) noexcept -> std::size_t {
    if (p.threads == 1 or total < 2 or total < p.threshold) { return 1; }

    auto n = p.threads;
    if (n == 0) {
      n = static_cast<std::size_t>(std::thread::hardware_concurrency());
      if (n == 0) { n = 1; }
    }
    return n < total ? n : total;
  }
}
