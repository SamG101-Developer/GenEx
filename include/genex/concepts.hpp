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

    template <typename U, typename Deleter>
    struct is_unique_ptr<std::unique_ptr<U, Deleter>> : std::true_type {
    };

    template <typename T>
    concept unique_ptr = is_unique_ptr<std::remove_cvref_t<T>>::value;
}
