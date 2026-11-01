module;
#include <genex/detail/macros.hpp>

export module genex.detail.temporary_buffer;
import std;

export namespace genex::detail {
  /// Storage for elements an algorithm moves out of a range while it works. If
  /// there's no memory for it, it's left empty rather than throwing, as the
  /// algorithms can all do the same job in place, just more slowly.
  ///
  /// Whatever was added is destroyed along with it, including if a comparison
  /// throws part way through.
  template <typename T>
  class temporary_buffer {
    T *m_data = nullptr;
    std::size_t m_capacity = 0;
    std::size_t m_size = 0;

  public:
    GENEX_INLINE constexpr explicit temporary_buffer(const std::size_t capacity) {
      try {
        m_data = std::allocator<T>{}.allocate(capacity);
        m_capacity = capacity;
      }
      catch (const std::bad_alloc &) {
      }
    }

    temporary_buffer(const temporary_buffer &) = delete;
    auto operator=(const temporary_buffer &) -> temporary_buffer& = delete;

    GENEX_INLINE constexpr ~temporary_buffer() {
      clear();
      if (m_data != nullptr) { std::allocator<T>{}.deallocate(m_data, m_capacity); }
    }

    [[nodiscard]] GENEX_INLINE constexpr auto data() const noexcept -> T* {
      return m_data;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto capacity() const noexcept -> std::size_t {
      return m_capacity;
    }

    [[nodiscard]] GENEX_INLINE constexpr auto size() const noexcept -> std::size_t {
      return m_size;
    }

    GENEX_INLINE constexpr auto push_back(T &&value) -> void {
      std::construct_at(m_data + m_size, std::move(value));
      ++m_size;
    }

    /// Fills the whole buffer with moved-from objects, without needing a default
    /// constructor, by passing @c seed from each to the next and then back to where
    /// it came from. An algorithm that's going to use the buffer many times can
    /// then assign into it, and not construct and destroy an element every time.
    constexpr auto fill_from(T &seed) -> void {
      if (m_capacity == 0) { return; }
      std::construct_at(m_data, std::move(seed));
      m_size = 1;
      for (; m_size < m_capacity; ++m_size) {
        std::construct_at(m_data + m_size, std::move(m_data[m_size - 1]));
      }
      seed = std::move(m_data[m_capacity - 1]);
    }

    GENEX_INLINE constexpr auto clear() noexcept -> void {
      if constexpr (not std::is_trivially_destructible_v<T>) {
        for (auto i = 0uz; i < m_size; ++i) { std::destroy_at(m_data + i); }
      }
      m_size = 0;
    }
  };
}
