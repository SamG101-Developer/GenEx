#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

#include "genex/categories.hpp"


#define EXPORT_GENEX_VIEW(name) \
    template <iterator I, sentinel S>\
    struct name ## _view final : name ## _fn, detail::view_base<I, S> { \
        explicit name ## _view(I &&first, S &&last)\
            : detail::view_base<I, S>(std::forward<I>(first), std::forward<S>(last)) {\
        }\
    };\
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_OUTPUT_TYPE(name) \
    template <typename Rng>\
    using output_type = name ## _view<iterators::begin_t<Rng>, iterators::end_t<Rng>>;

#define GENERATOR_TO_VIEW \
    return output_type<decltype(gen)>(iterators::begin(gen), iterators::end(gen))

#define DEFINE_VIEW(name) \
    template <iterator I, sentinel S>\
    struct name ## _view;\
    struct name ## _fn : detail::view_fn_base


#define FWD_TO_IMPL_VIEW(fn, ...) \
    auto gen = fn(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))));\
    GENERATOR_TO_VIEW

#define FWD_TO_IMPL_VIEW_VOID(fn, ...) \
    fn(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))));


namespace genex::views::detail {
    struct view_fn_base {
        virtual ~view_fn_base() = default;
    };

    template <concepts::iterator I, concepts::sentinel S> // , typename Cat = std::iterator_traits<I>::iterator_concept>
    struct view_base {
    public:
        using value_type = iter_value_t<I>;
        using iterator = I;
        using sentinel = S;
        using iterator_category = std::input_iterator_tag;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    protected:
        I m_begin;
        S m_end;

        explicit view_base(I &&begin, S &&end) :
            m_begin(std::forward<I>(begin)),
            m_end(std::forward<S>(end)) {
        }

    public:
        auto begin() && noexcept { return std::move(m_begin); }
        auto end() && noexcept { return std::move(m_end); }

        auto begin() & = delete;
        auto end() & = delete;

        virtual ~view_base() = default;
    };
}
