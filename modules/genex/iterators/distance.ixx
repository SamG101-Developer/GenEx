module;
#include <iterator>

export module genex.iterators.distance;


namespace genex::iterators {
    struct distance_fn {
        template <typename Rng>
        constexpr auto operator()(Rng &&rng) const noexcept -> size_t;
    };

    export inline constexpr distance_fn distance;
}


template <typename Rng>
constexpr auto genex::iterators::distance_fn::operator()(Rng &&rng) const noexcept -> size_t {
    size_t length = 0;
    for (auto &&x : rng) { ++length; }
    return length;
}
