module;
#include <genex/detail/macros.hpp>

export module genex.detail.bounded_random;
import std;

export namespace genex::detail {
  /// Draws uniform numbers below a bound from a generator, without bias and with
  /// as few draws and divisions as possible. A 64-bit generator gives two 32-bit
  /// numbers per draw, each turned into the result by a multiply: only a result
  /// that lands in the small biased region needs a division to tell, and then
  /// perhaps a redraw.
  template <typename G>
  class bounded_random {
    static constexpr auto range =
      static_cast<std::uint64_t>(G::max()) -
      static_cast<std::uint64_t>(G::min());

    G &m_generator;
    std::uint32_t m_spare = 0;
    bool m_has_spare = false;

    GENEX_INLINE constexpr auto draw() -> std::uint64_t {
      return
        static_cast<std::uint64_t>(m_generator()) -
        static_cast<std::uint64_t>(G::min());
    }

    GENEX_INLINE constexpr auto draw_32() -> std::uint32_t {
      if (m_has_spare) {
        m_has_spare = false;
        return m_spare;
      }
      const auto value = draw();
      m_spare = static_cast<std::uint32_t>(value);
      m_has_spare = true;
      return static_cast<std::uint32_t>(value >> 32);
    }

    GENEX_INLINE constexpr auto draw_32() -> std::uint32_t
    requires (range == std::numeric_limits<std::uint32_t>::max()) {
      return static_cast<std::uint32_t>(draw());
    }

    constexpr auto below_by_multiply(const std::uint32_t n) -> std::uint32_t {
      auto product = static_cast<std::uint64_t>(draw_32()) * n;
      auto low = static_cast<std::uint32_t>(product);
      if (low < n) [[unlikely]] {
        const auto threshold = static_cast<std::uint32_t>(0u - n) % n;
        while (low < threshold) {
          product = static_cast<std::uint64_t>(draw_32()) * n;
          low = static_cast<std::uint32_t>(product);
        }
      }
      return static_cast<std::uint32_t>(product >> 32);
    }

    constexpr auto below_by_rejection(const std::uint64_t n) -> std::uint64_t {
      // How many values of the generator's map to each result. A limit that
      // overflowed to zero means the generator's range divides evenly.
      const auto scale = range / n + (range % n == n - 1 ? 1 : 0);
      const auto limit = n * scale;
      auto value = draw();
      while (limit != 0 and value >= limit) { value = draw(); }
      return value / scale;
    }

    /// The generator's range is too narrow for one draw, so a draw picks the
    /// low part, and a smaller bound the high part. This could overshoot, as
    /// the top high part isn't a whole range, so those are thrown away.
    constexpr auto below_by_composing(const std::uint64_t n) -> std::uint64_t {
      const auto base = range + 1;
      while (true) {
        const auto high = below((n - 1) / base + 1) * base;
        const auto value = high + draw();
        if (value >= high and value < n) { return value; }
      }
    }

  public:
    GENEX_INLINE constexpr explicit bounded_random(G &generator) :
      m_generator(generator) {
    }

    /// A uniform number from 0 up to but not including @c n, which can't be 0.
    GENEX_INLINE constexpr auto below(const std::uint64_t n) -> std::uint64_t {
      if constexpr (range == std::numeric_limits<std::uint32_t>::max() or
        range == std::numeric_limits<std::uint64_t>::max()) {
        if (n <= std::numeric_limits<std::uint32_t>::max()) [[likely]] {
          return below_by_multiply(static_cast<std::uint32_t>(n));
        }
      }

      if (n - 1 <= range) { return below_by_rejection(n); }
      return below_by_composing(n);
    }
  };
}
