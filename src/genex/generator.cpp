module;
#include <genex/macros.hpp>

export module genex.generator;
import std;


namespace genex {
    template <typename T>
    using generator1 = std::generator<T>;

    template <typename T>
    struct GENEX_NODISCARD generator2;

    export template <typename T>
    using generator = generator1<T>;
}

template <typename T>
struct GENEX_NODISCARD genex::generator2 {
public:
    struct promise_type {
        using handle_t = std::coroutine_handle<promise_type>;
        std::remove_cvref_t<T> value;

        GENEX_NODISCARD GENEX_INLINE static auto operator new(const std::size_t n) -> void* {
            return ::operator new(n);
        }

        GENEX_INLINE static auto operator delete(void *const p) noexcept -> void {
            ::operator delete(p);
        }

        GENEX_NODISCARD GENEX_INLINE auto get_return_object() -> generator2 {
            return generator2(handle_t::from_promise(*this));
        }

        GENEX_NODISCARD GENEX_INLINE static auto initial_suspend() noexcept -> std::suspend_always {
            return {};
        }

        GENEX_INLINE static auto final_suspend() noexcept -> std::suspend_always {
            return {};
        }

        template <typename U> requires std::convertible_to<U, T>
        GENEX_NODISCARD GENEX_INLINE auto yield_value(U &&val) noexcept(std::is_nothrow_assignable_v<std::remove_cvref_t<T>&&, U&&>) -> std::suspend_always {
            value = std::forward<U>(val);
            return {};
        }

        GENEX_INLINE static auto return_void() noexcept -> void {}

        GENEX_NORETURN GENEX_INLINE static auto unhandled_exception() noexcept -> std::suspend_always {
            std::terminate();
        }
    };

    using handle_t = promise_type::handle_t;

    struct sentinel_t {};

    struct iterator_t {
        using value_type = std::remove_cvref_t<T>;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::input_iterator_tag;

        handle_t h{};

        GENEX_INLINE constexpr iterator_t() noexcept = default;
        GENEX_INLINE constexpr explicit iterator_t(const handle_t h) noexcept : h(h) {}

        GENEX_NODISCARD GENEX_INLINE constexpr auto operator*() const noexcept -> reference {
            return h.promise().value;
        }

        GENEX_NODISCARD GENEX_INLINE constexpr auto operator->() const noexcept -> reference {
            return &h.promise().value;
        }

        GENEX_INLINE constexpr auto operator++() noexcept -> iterator_t {
            h.resume();
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept -> void {
            ++*this;
        }

        GENEX_NODISCARD GENEX_INLINE friend constexpr auto operator==(iterator_t const &it, const sentinel_t) noexcept -> bool {
            return it.h.done();
        }

        GENEX_NODISCARD GENEX_INLINE friend constexpr auto operator==(const sentinel_t, iterator_t const &it) noexcept -> bool {
            return it.h.done();
        }
    };

    GENEX_INLINE constexpr explicit generator2(const handle_t h) noexcept : m_h(h) {}

    generator2(generator2 const &) = delete;
    auto operator=(generator2 const &) -> generator2& = delete;

    GENEX_INLINE constexpr generator2(generator2 &&other) noexcept : m_h(std::exchange(other.m_h, {})) {}

    GENEX_INLINE constexpr auto operator=(generator2 &&other) noexcept -> generator2& {
        if (this != std::addressof(other)) {
            if (m_h) { m_h.destroy(); }
            m_h = std::exchange(other.m_h, {});
        }
        return *this;
    }

    GENEX_INLINE ~generator2() noexcept {
        if (m_h) { m_h.destroy(); }
    }

    GENEX_NODISCARD GENEX_INLINE auto begin() noexcept -> iterator_t {
        if (m_h) { m_h.resume(); }
        return iterator_t{m_h};
    }

    GENEX_NODISCARD GENEX_INLINE auto end() noexcept -> sentinel_t {
        return {};
    }

private:
    handle_t m_h;
};
