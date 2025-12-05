module;
#include <genex/macros.hpp>

export module genex.views2.ptr;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.views2.transform;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept ptr_gettable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        (unique_ptr<iter_value_t<I>> or shared_ptr<iter_value_t<I>> or weak_ptr<iter_value_t<I>>);

    template <typename Rng>
    concept ptr_gettable_range =
        input_range<Rng> and
        ptr_gettable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2 {
    struct ptr_fn {
        template <typename I, typename S>
        requires detail::concepts::ptr_gettable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return transform(std::move(first), std::move(last), [](auto &&ptr) { return ptr.get(); });
        }

        template <typename Rng>
        requires detail::concepts::ptr_gettable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return transform(std::move(first), std::move(last), [](auto &&ptr) { return ptr.get(); });
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(ptr_fn{});
        }
    };

    export inline constexpr ptr_fn ptr{};
}
