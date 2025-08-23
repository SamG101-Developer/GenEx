#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


#define EXPORT_GENEX_VIEW(name) \
    template <range Rng> \
    struct name ## _view final : name ## _fn, detail::view_base<Rng> { \
        explicit name ## _view(Rng &&rng) \
            : detail::view_base<Rng>(std::forward<Rng>(rng)) { \
        } \
    }; \
    EXPORT_GENEX_STRUCT(name)

#define DEFINE_OUTPUT_TYPE(name) \
    template <typename Rng> \
    using output_type = name ## _view<Rng>;

#define GENERATOR_TO_VIEW \
    return output_type<decltype(gen)>(std::move(gen))

#define DEFINE_VIEW(name) \
    template <range Rng> \
    struct name ## _view; \
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

    template <range Rng> // , typename Cat = std::iterator_traits<I>::iterator_concept>
    struct view_base {
    public:
        using value_type = range_value_t<Rng>;
        using iterator = iterators::begin_t<Rng>;
        using sentinel = iterators::end_t<Rng>;
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

        auto get_generator() && {
            return std::move(m_rng);
        }

        virtual ~view_base() = default;
    };
}
