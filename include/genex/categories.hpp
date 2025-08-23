#pragma once
#include <iterator>

#include <genex/concepts.hpp>


namespace genex::categories {
    template <typename T>
    concept input_iterator = iterator<T> and std::input_iterator<T>;

    template <typename T>
    concept forward_iterator = iterator<T> and std::forward_iterator<T>;

    template <typename T>
    concept bidirectional_iterator = iterator<T> and std::bidirectional_iterator<T>;

    template <typename T>
    concept random_access_iterator = iterator<T> and std::random_access_iterator<T>;

    template <typename T>
    concept contiguous_iterator = iterator<T> and std::contiguous_iterator<T>;

    template <typename Rng>
    concept input_range = concepts::range<Rng> and input_iterator<iterators::begin_t<Rng>>;

    template <typename Rng>
    concept forward_range = concepts::range<Rng> and forward_iterator<iterators::begin_t<Rng>>;

    template <typename Rng>
    concept bidirectional_range = concepts::range<Rng> and bidirectional_iterator<iterators::begin_t<Rng>>;

    template <typename Rng>
    concept random_access_range = concepts::range<Rng> and random_access_iterator<iterators::begin_t<Rng>>;

    template <typename Rng>
    concept contiguous_range = concepts::range<Rng> and contiguous_iterator<iterators::begin_t<Rng>>;

    // template <typename Cat1, typename Cat2>
    // struct min_iterator_category;
    //
    // template <typename... Iterators>
    // struct common_iterator_category;
    //
    // template <typename First, typename Second, typename... Rest>
    // struct common_iterator_category<First, Second, Rest...>;
    //
    // template <typename... Iterators>
    // using common_iterator_category_t = typename common_iterator_category<Iterators...>::type;

    // template <typename Rng>
    // concept non_generator_range = range<Rng> and random_access_iterator<Rng>;
    //
    // template <typename Rng>
    // concept generator_range = range<Rng> and input_iterator<Rng> and not random_access_iterator<Rng>;

    // template <typename T>
    // using iterator_traits = std::iterator_traits<T>;
}


// template <typename Cat1, typename Cat2>
// struct genex::categories::min_iterator_category {
//     using type = std::conditional_t<std::is_base_of_v<Cat2, Cat1>, Cat1, Cat2>;
// };
//
//
// template <typename First, typename Second, typename... Rest>
// struct genex::categories::common_iterator_category<First, Second, Rest...> {
// private:
//     using cat1 = typename std::iterator_traits<First>::iterator_category;
//     using cat2 = typename std::iterator_traits<Second>::iterator_category;
//     using weaker = typename min_iterator_category<cat1, cat2>::type;
//
// public:
//     using type = common_iterator_category_t<std::type_identity_t<weaker>, Rest...>::type;
// };
