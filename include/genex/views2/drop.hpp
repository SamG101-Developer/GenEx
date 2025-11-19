#pragma once
#include <genex/views2/view_base.hpp>


namespace genex::views2::detail::concepts {
    template <typename I, typename S, typename Int>
    concept droppable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int>;

    template <typename Rng, typename Int>
    concept droppable_range =
        input_range<Rng> and
        droppable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int>;
}


namespace genex::views2::detail::impl {
    struct drop_sentinel {};

    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    struct drop_iterator : view_iterator_base {
        I it; S st;
        Int n;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_FORWARD_ITERATOR_OPERATIONS;

        GENEX_INLINE constexpr drop_iterator() = default;

        GENEX_INLINE constexpr drop_iterator(I first, S last, Int n) :
            it(std::move(first)), st(std::move(last)), n(std::move(n)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(drop_sentinel) {
            return self.it == self.st;
        }

    private:
        template <typename Self>
        requires (not std::random_access_iterator<I>)
        GENEX_INLINE constexpr auto fwd_to_valid(this Self&& self) -> void {
            for (auto m = self.n; m > 0 and self.it != self.st; --m, ++self.it);
        }

        template <typename Self>
        requires std::random_access_iterator<I>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self&& self) -> void {
            self.it += self.n;
        }
    };

    template <typename I, typename S, typename Int>
    requires concepts::droppable_iters<I, S, Int>
    struct drop_view : view_base {
        I it; S st;
        Int n;

        GENEX_INLINE constexpr drop_view(I first, S last, Int n) :
            it(std::move(first)), st(std::move(last)), n(std::move(n)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return drop_iterator{self.it, self.st, self.n};
        }

        template <typename Self>
        GENEX_ITER_END {
            return drop_sentinel{};
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st) - self.n;
        }
    };
}


namespace genex::views2 {
    struct drop_fn {
        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_iters<I, S, Int> and std::random_access_iterator<I>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            return genex::span<iter_value_t<I>>(first + n, last);
        }

        template <typename I, typename S, typename Int>
        requires detail::concepts::droppable_iters<I, S, Int>
        GENEX_INLINE constexpr auto operator()(I first, S last, const Int n) const {
            return detail::impl::drop_view(std::move(first), std::move(last), std::move(n));
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int> and std::random_access_iterator<iterator_t<Rng>>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return genex::span<range_value_t<Rng>>(first + n, last);
        }

        template <typename Rng, typename Int>
        requires detail::concepts::droppable_range<Rng, Int>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, const Int n) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::drop_view(std::move(first), std::move(last), std::move(n));
        }

        template <typename Int>
        requires std::weakly_incrementable<Int>
        GENEX_INLINE constexpr auto operator()(const Int n) const {
            return meta::bind_back(drop_fn{}, std::move(n));
        }
    };

    inline constexpr drop_fn drop{};
}
