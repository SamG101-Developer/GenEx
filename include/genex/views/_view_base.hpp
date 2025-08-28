#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>


#define EXPORT_GENEX_VIEW(name) \
    template <typename Rng> requires input_range<Rng> \
    struct name ## _view final : name ## _fn, genex::views::detail::view_base<Rng> { \
        explicit name ## _view(Rng &&rng) \
            : genex::views::detail::view_base<Rng>(std::forward<Rng>(rng)) { \
        } \
    }; \
    EXPORT_GENEX_STRUCT(name)


#define DEFINE_VIEW(name) \
    template <typename Rng> requires input_range<Rng> \
    struct name ## _view; \
    struct name ## _fn : genex::views::detail::view_fn_base


namespace genex::views::detail {
    struct view_fn_base {
        virtual ~view_fn_base() = default;
    };

    template <typename Rng> requires range<Rng>
    struct view_base {
    public:
        using value_type = range_value_t<Rng>;
        using iterator = iterator_t<Rng>;
        using sentinel = sentinel_t<Rng>;
        using difference_type = std::ptrdiff_t;
        using reference = range_reference_t<Rng>;
        using const_reference = const reference;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;

    protected:
        [[no_unique_address]]
        Rng m_rng;

        explicit view_base(Rng &&rng) :
            m_rng(std::forward<Rng>(rng)) {
        }

    public:
        auto begin() { return iterators::begin(m_rng); }
        auto end() { return iterators::end(m_rng); }

        auto begin() const requires range<const Rng> { return iterators::begin(m_rng); }
        auto end() const requires range<const Rng> { return iterators::end(m_rng); }

        auto operator[](size_type n) -> reference requires random_access_range<Rng> {
            return m_rng[n];
        }

        virtual ~view_base() = default;
    };
}
