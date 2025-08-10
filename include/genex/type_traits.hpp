#pragma once
#include <type_traits>
#include <genex/generator.hpp>
#include <genex/iterators/begin.hpp>


namespace genex::type_traits {
    template <typename Rng>
    struct range_value;

    template <typename I>
    struct iter_value;

    template <typename T>
    struct deref_value;

    template <typename Rng>
    using range_value_t = typename range_value<std::remove_cvref_t<Rng>>::type;

    template <typename I>
    using iter_value_t = typename iter_value<I>::type;

    template <typename T>
    using deref_value_t = typename deref_value<T>::type;
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
    using type = typename decltype(std::declval<generator<T>>().begin())::value_type;
};


template <typename Rng> requires (genex::iterators::has_begin<Rng> and not requires { typename Rng::value_type; })
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
    using type = const std::remove_cvref_t<decltype(*std::declval<I const*>())>;
};


template <typename I> requires requires(I &&i) { *i; }
struct genex::type_traits::iter_value<I const> {
    using type = const std::remove_cvref_t<decltype(*std::declval<I const>())>;
};


template <typename T> requires requires(T const &t) { *std::declval<T>(); }
struct genex::type_traits::deref_value<T> {
    using type = std::remove_cvref_t<decltype(*std::declval<T>())>;
};
