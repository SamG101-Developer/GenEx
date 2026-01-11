module;
#include <genex/macros.hpp>

export module genex.views2.split;
export import genex.pipe;
import genex.algorithms.count;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename E>
    concept splittable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*>;;

    template <typename Rng, typename E>
    concept splittable_range =
        forward_range<Rng> and
        splittable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}


namespace genex::views {
    struct split_sentinel {};

    template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
    struct split_iterator {
        I it; S st;
        E elem;

        using value_type = std::ranges::subrange<I>;
        using reference_type = std::ranges::subrange<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category =
            std::conditional_t<std::random_access_iterator<I>, std::random_access_iterator_tag,
            std::conditional_t<std::bidirectional_iterator<I>, std::bidirectional_iterator_tag,
            std::forward_iterator_tag>>;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(split_iterator);

        GENEX_INLINE constexpr split_iterator() = default;

        GENEX_INLINE constexpr split_iterator(I first, S last, E e) :
            it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_NEXT {
            while (self.it != self.st && not operations::eq{}(*self.it, self.elem)) { ++self.it; }
            if (self.it != self.st) { ++self.it; }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_PREV {
            while (self.it != self.st) {
                --self.it;
                if (operations::eq{}(*self.it, self.elem)) { ++self.it; break; }
            }
            return self;
        }

        template <typename Self>
        GENEX_VIEW_CUSTOM_DEREF {
            auto end_it = self.it;
            while (end_it != self.st && not operations::eq{}(*end_it, self.elem)) { ++end_it; }
            return std::ranges::subrange(self.it, end_it);
        }

        template <typename Self>
        GENEX_VIEW_ITER_EQ(split_sentinel) {
            return self.it == self.st;
        }
    };

    template <typename I, typename S, typename E>
    requires detail::concepts::splittable_iters<I, S, E>
    struct split_view {
        I it; S st;
        E elem;

        GENEX_INLINE constexpr split_view(I first, S last, E e) :
            it(std::move(first)), st(std::move(last)), elem(std::move(e)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return split_iterator(self.it, self.st, self.elem);
        }

        template <typename Self>
        GENEX_ITER_END {
            return split_sentinel();
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            auto elem_count = genex::count(self.it, self.st, self.elem);
            return elem_count == 0 ? 1 : elem_count + 1;
        }
    };
}


namespace genex::views {
    struct split_fn {
        template <typename I, typename S, typename E>
        requires detail::concepts::splittable_iters<I, S, E>
        GENEX_INLINE constexpr auto operator()(I first, S last, E elem) const -> split_view<I, S, E> {
            return split_view(std::move(first), std::move(last), std::move(elem));
        }

        template <typename Rng, typename E>
        requires detail::concepts::splittable_range<Rng, E>
        GENEX_INLINE constexpr auto operator()(Rng&& rng, E elem) const -> split_view<iterator_t<Rng>, sentinel_t<Rng>, E> {
            auto [first, last] = iterators::iter_pair(rng);
            return split_view(std::move(first), std::move(last), std::move(elem));
        }

        template <typename E>
        requires (not range<E>)
        GENEX_INLINE constexpr auto operator()(E elem) const -> auto {
            return meta::bind_back(split_fn{}, std::move(elem));
        }
    };

    export inline constexpr split_fn split{};
}
