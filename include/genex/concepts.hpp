#pragma once
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
}
