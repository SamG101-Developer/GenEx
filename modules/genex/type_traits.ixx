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


template <typename Rng> requires requires { typename Rng::value_type; }
struct genex::type_traits::range_value<Rng> {
    using type = typename Rng::value_type;
};


template <typename Rng> requires requires { typename Rng::element_type; }
struct genex::type_traits::range_value<Rng> {
    using type = typename Rng::element_type;
};


template <typename T>
struct genex::type_traits::range_value<genex::generator<T>> {
    using type = typename generator<T>::iterator::value_type;
};


template <typename Rng> requires genex::iterators::has_begin<Rng> and not requires { typename Rng::value_type; }
struct genex::type_traits::range_value<Rng> {
    using type = iterators::begin_t<Rng>;
};


template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I> {
    using type = std::remove_cvref_t<decltype(*std::declval<I>())>;
};


template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I*> {
    using type = std::remove_cvref_t<decltype(*std::declval<I*>())>;
};


template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I const*> {
    using type = std::remove_cvref_t<decltype(*std::declval<I const*>())>;
};


template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I const> {
    using type = std::remove_cvref_t<decltype(*std::declval<I const>())>;
};
