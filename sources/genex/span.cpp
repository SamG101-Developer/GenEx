module;
#include <genex/detail/macros.hpp>

export module genex.span;
import genex.concepts;
import genex.iterators.access;
import std;

namespace genex {
  export template <typename T>
  class span {
    T *m_ptr = nullptr;
    std::size_t m_size = 0;

  public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using iterator = T*;
    using const_iterator = const T*;
    using sentinel = T*;
    using const_sentinel = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    GENEX_INLINE constexpr span() = default;

    GENEX_INLINE constexpr span(T *begin, const std::size_t size) noexcept :
      m_ptr(begin), m_size(size) {
    }

    template <typename It, typename St>
    requires std::contiguous_iterator<It> and std::sized_sentinel_for<St, It>
    GENEX_INLINE constexpr span(It begin, St end) noexcept(
      SAFE_NOEX(end - begin)) :
      m_ptr(std::to_address(begin)),
      m_size(static_cast<std::size_t>(end - begin)) {
    }

    /// A view of a contiguous range's elements. An rvalue range must
    /// outlive the span, so only a borrowed one, or a span of const
    /// elements (bound for the length of a call), is allowed.
    template <typename Rng>
      requires (not std::same_as<std::remove_cvref_t<Rng>, span>)
      and contiguous_range<Rng> and sized_range<Rng>
      and std::convertible_to<std::remove_reference_t<range_reference_t<Rng>>(*)[], T(*)[]>
      and (non_dangling_range<Rng> or std::is_const_v<T>)
    GENEX_INLINE constexpr span(Rng &&rng) noexcept(
      SAFE_CALL(decltype(iterators::begin), Rng&) and
      SAFE_CALL(decltype(iterators::end), Rng&) and
      SAFE_CTOR(span, iterator_t<Rng>, sentinel_t<Rng>)) :
      span(iterators::begin(rng), iterators::end(rng)) {
    }

    /// A span of "U" as a span of "T", e.g. "U" to "U const".
    template <typename U>
    requires (not std::same_as<U, T>) and std::convertible_to<U(*)[], T(*)[]>
    GENEX_INLINE constexpr span(span<U> const &other) noexcept :
      m_ptr(other.data()), m_size(other.size()) {
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto data(this Self &&self) noexcept -> T* {
      return self.m_ptr;
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto size(this Self &&self) noexcept -> std::size_t {
      return self.m_size;
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto empty(this Self &&self) noexcept -> bool {
      return self.m_size == 0;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto begin() const {
      return m_ptr;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto end() const {
      return m_ptr + m_size;
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto operator[](this Self &&self, const std::size_t index) noexcept -> T& {
      return self.m_ptr[index];
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto front(this Self &&self) noexcept -> T& {
      return *self.m_ptr;
    }

    template <typename Self>
    [[nodiscard]] GENEX_INLINE constexpr auto back(this Self &&self) noexcept -> T& {
      return *(self.m_ptr + self.m_size - 1);
    }

    /// The first "count" elements.
    [[nodiscard]] GENEX_INLINE constexpr auto first(const std::size_t count) const noexcept -> span {
      return span(m_ptr, count);
    }

    /// The last "count" elements.
    [[nodiscard]] GENEX_INLINE constexpr auto last(const std::size_t count) const noexcept -> span {
      return span(m_ptr + (m_size - count), count);
    }

    /// The "count" elements from "offset", or every element from it.
    [[nodiscard]] GENEX_INLINE constexpr auto subspan(
      const std::size_t offset, const std::size_t count = std::dynamic_extent) const noexcept -> span {
      return span(m_ptr + offset, count == std::dynamic_extent ? m_size - offset : count);
    }
  };
}

template <typename T>
inline constexpr auto genex::enable_borrowed_range<genex::span<T>> = true;

template <typename T>
inline constexpr auto genex::enable_view<genex::span<T>> = true;
