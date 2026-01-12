module;
#include <genex/macros.hpp>

export module genex.views2.remove;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept removable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<E> const*>;

    template <typename Rng, typename E, typename Proj>
    concept removable_range =
        input_range<Rng> and
        removable_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;
}


namespace genex::views::detail {
    struct remove_sentinel{};

    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    struct remove_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS E value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(remove_iterator)

        GENEX_INLINE constexpr remove_iterator() = default;

        GENEX_INLINE constexpr remove_iterator(I first, S last, E val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            value(std::move(val)), proj(std::move(proj)) {
            fwd_to_valid();
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            ++self.it;
            self.fwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            --self.it;
            self.bwd_to_valid();
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            return *self.it;
        }

        GENEX_VIEW_ITER_EQ(remove_iterator, remove_iterator) {
            return self.it == that.it;
        }

        GENEX_VIEW_ITER_EQ(remove_iterator, remove_sentinel) {
            return self.it == self.st;
        }

    private:
        template <typename Self>
        GENEX_INLINE constexpr auto fwd_to_valid(this Self&& self) -> void {
            while (self.it != self.st and operations::eq{}(std::invoke(self.proj, *self.it), self.value)) { ++self.it; }
        }

        template <typename Self>
        GENEX_INLINE constexpr auto bwd_to_valid(this Self&& self) -> void {
            while (self.it != self.st and operations::eq{}(std::invoke(self.proj, *self.it), self.value)) { --self.it; }
        }
    };


    template <typename I, typename S, typename E, typename Proj>
    requires concepts::removable_iters<I, S, E, Proj>
    struct remove_view {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS E value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr remove_view(I first, S last, E val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            value(std::move(val)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return remove_iterator{self.it, self.st, self.value, self.proj};
        }

        template <typename Self>
        GENEX_ITER_END {
            return remove_sentinel();
        }
    };
}


namespace genex::views {
    struct remove_fn {
        template <typename I, typename S, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_iters<I, S, E, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, E value, Proj proj = {}) const {
            return detail::remove_view(std::move(first), std::move(last), std::move(value), std::move(proj));
        }

        template <typename Rng, typename E, typename Proj = meta::identity>
        requires detail::concepts::removable_range<Rng, E, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, E value, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::remove_view(std::move(first), std::move(last), std::move(value), std::move(proj));
        }

        template <typename E, typename Proj = meta::identity>
        requires (not range<E>)
        GENEX_INLINE constexpr auto operator()(E value, Proj proj = {}) const {
            return meta::bind_back(remove_fn{}, std::move(value), std::move(proj));
        }
    };

    export inline constexpr remove_fn remove{};
}
