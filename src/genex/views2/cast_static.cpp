module;
#include <genex/macros.hpp>

export module genex.views2.cast_static;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename To, typename I, typename S>
    concept static_castable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        requires(iter_value_t<I> &&from) { static_cast<To>(from); };

    template <typename To, typename Rng>
    concept static_castable_range =
        input_range<Rng> and
        static_castable_iters<To, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views2::detail::impl {
    template <typename To, typename I, typename S>
    requires concepts::static_castable_iters<To, I, S>
    struct cast_static_iterator {
        I it;

        using value_type = To;
        using reference_type = To;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(cast_static_iterator);

        GENEX_INLINE constexpr cast_static_iterator() = default;

        GENEX_INLINE constexpr explicit cast_static_iterator(I first) :
            it(std::move(first)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return static_cast<To>(*self.it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(cast_static_iterator) {
            return self.it == that.it;
        }
    };

    template <typename To, typename I, typename S>
    requires concepts::static_castable_iters<To, I, S>
    struct cast_static_view {
        I it;
        S st;

        GENEX_INLINE constexpr cast_static_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return cast_static_iterator<To, I, S>(self.it);
        }

        template <typename Self>
        GENEX_ITER_END {
            return cast_static_iterator<To, I, S>(self.st);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views2 {
    template <typename To>
    struct cast_static_fn {
        template <typename I, typename S>
        requires detail::concepts::static_castable_iters<To, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::cast_static_view<To, I, S>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::static_castable_range<To, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::cast_static_view<To, iterator_t<Rng>, sentinel_t<Rng>>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(cast_static_fn{});
        }
    };

    export template <typename To>
    inline constexpr cast_static_fn<To> cast_static{};
}
