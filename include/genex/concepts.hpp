#pragma once
#include <memory>
#include <genex/type_traits.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::type_traits;


namespace genex::concepts {
    template <typename Rng>
    concept range = iterators::has_begin<Rng> and iterators::has_end<Rng>;

    template <typename I>
    concept iterator = requires(I i)
    {
        { *i } -> std::convertible_to<iter_value_t<I>>;
        { ++i } -> std::same_as<I&>;
    };

    template <typename S>
    concept sentinel = requires(S s)
    {
        true;
        // { s == s } -> std::convertible_to<bool>;
        // { s != s } -> std::convertible_to<bool>;
    };

    template <typename T>
    struct is_unique_ptr : std::false_type {
    };

    template <typename T, typename Deleter>
    struct is_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {
    };

    template <typename T>
    concept unique_ptr = is_unique_ptr<std::remove_cvref_t<T>>::value;


    template <typename T>
    struct is_shared_ptr : std::false_type {
    };

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {
    };

    template <typename T>
    concept shared_ptr = is_shared_ptr<std::remove_cvref_t<T>>::value;


    template <typename T>
    struct is_weak_ptr : std::false_type {
    };

    template <typename T>
    struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {
    };

    template <typename T>
    concept weak_ptr = is_weak_ptr<std::remove_cvref_t<T>>::value;

    template <typename S, typename I>
    concept sentinel_for = iterator<I> and sentinel<S> and std::sentinel_for<S, I>;
}
