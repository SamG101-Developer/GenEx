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

    template <typename Rng> requires input_range<Rng>
    struct view_base {
    public:
        using value_type = range_value_t<Rng>;
        using iterator = iterator_t<Rng>;
        using sentinel = sentinel_t<Rng>;
        using iterator_category = std::input_iterator_tag;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    protected:
        Rng m_rng;

        explicit view_base(Rng &&rng) :
            m_rng(std::forward<Rng>(rng)) {
        }

    public:
        auto begin() { return iterators::begin(m_rng); }
        auto end() { return iterators::end(m_rng); }

        virtual ~view_base() = default;
    };
}
