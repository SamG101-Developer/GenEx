module;
#include <genex/macros.hpp>

export module genex.views2.join;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.access;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import std;


namespace genex::views2::detail::concepts {
    template <typename I, typename S>
    concept joinable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    input_range<iter_value_t<I>>;

    template <typename Rng>
    concept joinable_range =
        input_range<Rng> and
        joinable_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}



namespace genex::views2::detail::impl {
    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    struct join_iterator {
        I it; S st;
        iterator_t<iter_value_t<I>> inner_it;
        sentinel_t<iter_value_t<I>> inner_st;

        using value_type = range_value_t<iter_value_t<I>>;
        using reference_type = range_reference_t<iter_value_t<I>>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(join_iterator)

        GENEX_INLINE constexpr join_iterator() = default;

        GENEX_INLINE constexpr join_iterator(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.inner_it;
            if (self.inner_it == self.inner_st) {
                ++self.it;
                self.fwd_to_valid();
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV = delete;

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            GENEX_ASSERT(std::runtime_error, self.it != self.st);
            GENEX_ASSERT(std::runtime_error, self.inner_it != iterator_t<iter_value_t<I>>{});
            return *self.inner_it;
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(join_iterator) {
            if (self.it != that.it) return false;
            if (self.it == self.st) return true;
            return self.inner_it == that.inner_it;
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self&& self) -> void {
            while (self.it != self.st) {
                self.inner_it = iterators::begin(*self.it);
                self.inner_st = iterators::end(*self.it);
                if (self.inner_it != self.inner_st) { break; }
                ++self.it;
            }
            self.inner_it = iterator_t<iter_value_t<I>>{};
            self.inner_st = sentinel_t<iter_value_t<I>>{};
        }
    };

    template <typename I, typename S>
    requires concepts::joinable_iters<I, S>
    struct join_view {
        I it; S st;

        GENEX_INLINE constexpr join_view(I first, S last) :
            it(std::move(first)), st(std::move(last)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return join_iterator(self.it, self.st);
        }

        template <typename Self>
        GENEX_ITER_END {
            return join_iterator(self.st, self.st);
        }

        template <typename Self>
        GENEX_ITER_SIZE requires std::forward_iterator<I> {
            std::size_t total_size = 0;
            for (auto it = self.it; it != self.st; ++it) {
                total_size += iterators::distance(iterators::begin(*it), iterators::end(*it));
            }
            return total_size;
        }
    };
}


namespace genex::views2 {
    struct join_fn {
        template <typename I, typename S>
        requires detail::concepts::joinable_iters<I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::join_view(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::joinable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::join_view(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(join_fn{});
        }
    };

    export inline constexpr join_fn join{};
}
