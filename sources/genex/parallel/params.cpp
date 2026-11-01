export module genex.parallel.params;
import std;

namespace genex::parallel {
  /// Tuning knobs shared by every parallel algorithm. The defaults are
  /// deliberately conservative: parallelism is only ever a win when the
  /// per-element work is expensive enough to pay for the thread spawn (roughly
  /// 10-30us each on Linux), so small ranges silently run on the calling
  /// thread.
  ///
  ///  - "threads" is the total number of workers, including the calling thread,
  ///    which always takes the first chunk. "0" means "ask the hardware"; "1"
  ///    forces the serial path.
  ///  - "threshold" is the element count below which the range is not split at
  ///    all. Set it to "0" to always split (useful in benchmarks, and when the
  ///    callable is known to be very expensive).
  export struct params {
    std::size_t threads = 0;
    std::size_t threshold = 1uz << 14;
  };
}
