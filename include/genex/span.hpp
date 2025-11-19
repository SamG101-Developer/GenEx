#pragma once
#include <cstddef>

#include <genex/macros.hpp>


namespace genex {
    template <typename T>
    class span {
    private:
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
        GENEX_INLINE constexpr span(It begin, St end) noexcept :
            m_ptr(std::addressof(*begin)),
            m_size(static_cast<std::size_t>(end - begin)) {
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto data(this Self &&self) noexcept -> T* {
            return self.m_ptr;
        }

        template <typename Self>
        GENEX_NODISCARD GENEX_INLINE constexpr auto size(this Self &&self) noexcept -> std::size_t {
            return self.m_size;
        }

        template <typename Self>
        GENEX_ITER_EMPTY {
            return self.m_size == 0;
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return self.m_ptr;
        }

        template <typename Self>
        GENEX_ITER_END {
            return self.m_ptr + self.m_size;
        }

        template <typename Self>
        GENEX_INLINE constexpr auto operator[](this Self &&self, const std::size_t index) noexcept -> T& {
            return self.m_ptr[index];
        }

        template <typename Self>
        GENEX_INLINE auto front(this Self &&self) noexcept -> T& {
            return *self.m_ptr;
        }

        template <typename Self>
        GENEX_INLINE auto back(this Self &&self) noexcept -> T& {
            return *(self.m_ptr + self.m_size - 1);
        }
    };
}
