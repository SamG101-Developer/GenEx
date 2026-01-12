module;
#include <genex/macros.hpp>

export module genex.views2.join_with;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename New>
    concept joinable_with_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        input_range<iter_value_t<I>> and
        std::convertible_to<New, range_value_t<iter_value_t<I>>>;

    template <typename Rng, typename New>
    concept joinable_with_range =
        forward_range<Rng> and
        joinable_with_iters<iterator_t<Rng>, sentinel_t<Rng>, New>;
}


namespace genex::views::detail::impl {
    struct join_sentinel {};

    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    struct join_iterator {
        I it;
        S st;
        New new_value;
        bool use_new = false;
        iterator_t<iter_value_t<I>> inner_it;
        sentinel_t<iter_value_t<I>> inner_st;

        using value_type = range_value_t<iter_value_t<I>>;
        using reference_type = range_reference_t<iter_value_t<I>>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::conditional_t<
            std::forward_iterator<I>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(join_iterator)

        GENEX_INLINE constexpr join_iterator() = default;

        GENEX_INLINE constexpr join_iterator(I first, S last, New new_val) :
            it(std::move(first)), st(std::move(last)), new_value(std::move(new_val)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            if (self.it == self.st) { return self; }
            ++self.inner_it;
            if (self.use_new) {
                self.use_new = false;
                ++self.it;
                self.fwd_to_valid();
                return self;
            }
            if (self.inner_it == self.inner_st) {
                self.use_new = true;
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV = delete;

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            GENEX_ASSERT(std::runtime_error, self.it != self.st);
            GENEX_ASSERT(std::runtime_error, self.inner_it != self.inner_st or self.use_new);
            if (self.use_new) { return *&self.new_value; }
            return *self.inner_it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(join_sentinel) {
            if (self.it == self.st) { return true; }

            if (self.use_new) {
                auto next_it = self.it;
                ++next_it;
                return next_it == self.st;
            }

            return false;
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self &&self) -> void {
            while (self.it != self.st) {
                self.inner_it = iterators::begin(*self.it);
                self.inner_st = iterators::end(*self.it);
                if (self.inner_it != self.inner_st) { return; }
                ++self.it;
            }
            self.inner_it = iterator_t<iter_value_t<I>>{};
            self.inner_st = sentinel_t<iter_value_t<I>>{};
        }
    };

    template <typename I, typename S, typename New>
    requires concepts::joinable_with_iters<I, S, New>
    struct join_view {
        I it;
        S st;
        New new_value;

        GENEX_INLINE constexpr join_view(I first, S last, New new_val) :
            it(std::move(first)), st(std::move(last)), new_value(std::move(new_val)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return join_iterator(self.it, self.st, self.new_value);
        }

        template <typename Self>
        GENEX_ITER_END {
            return join_sentinel();
        }

        template <typename Self>
        GENEX_ITER_SIZE requires std::forward_iterator<I> {
            auto total_size = 0z;
            for (auto it = self.it; it != self.st; ++it) {
                total_size += iterators::distance(iterators::begin(*it), iterators::end(*it));
            }
            total_size += std::max(0z, iterators::distance(self.it, self.st) - 1z);
            return total_size;
        }
    };
}


namespace genex::views {
    struct join_with_fn {
        template <typename I, typename S, typename New>
        requires detail::concepts::joinable_with_iters<I, S, New>
        GENEX_INLINE constexpr auto operator()(I first, S last, New new_value) const {
            return detail::impl::join_view(std::move(first), std::move(last), std::move(new_value));
        }

        template <typename Rng, typename New>
        requires detail::concepts::joinable_with_range<Rng, New>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, New new_value) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::join_view(std::move(first), std::move(last), std::move(new_value));
        }

        template <typename New>
        requires (not range<New>)
        GENEX_INLINE constexpr auto operator()(New new_value) const {
            return meta::bind_back(join_with_fn{}, std::move(new_value));
        }
    };

    export inline constexpr join_with_fn join_with{};
}
