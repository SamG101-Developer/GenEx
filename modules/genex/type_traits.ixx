module;
#include <type_traits>


export module genex.type_traits;
import genex.iterators.begin;
import genex.generator;


namespace genex::type_traits {
    template <typename Rng>
    struct range_value;

    export template <typename I>
    struct iter_value;

    export template <typename Rng>
    using range_value_t = typename range_value<std::remove_cvref_t<Rng>>::type;

    export template <typename I>
    using iter_value_t = typename iter_value<I>::type;
}


export template <typename Rng> requires requires { typename Rng::value_type; }
struct genex::type_traits::range_value<Rng> {
    using type = typename Rng::value_type;
};


export template <typename Rng> requires requires { typename Rng::element_type; }
struct genex::type_traits::range_value<Rng> {
    using type = typename Rng::element_type;
};


export template <typename T>
struct genex::type_traits::range_value<genex::generator<T>> {
    using type = typename decltype(std::declval<generator<T>>().begin())::value_type;
};


export template <typename Rng> requires (genex::iterators::has_begin<Rng> and not requires { typename Rng::value_type; })
struct genex::type_traits::range_value<Rng> {
    using type = iterators::begin_t<Rng>;
};


export template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I> {
    using type = std::remove_cvref_t<decltype(*std::declval<I>())>;
};


export template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I*> {
    using type = std::remove_cvref_t<decltype(*std::declval<I*>())>;
};


export template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I const*> {
    using type = const std::remove_cvref_t<decltype(*std::declval<I const*>())>;
};


export template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I const> {
    using type = const std::remove_cvref_t<decltype(*std::declval<I const>())>;
};
