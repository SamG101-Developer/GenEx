module;
#include <genex/macros.hpp>

export module genex.views2.replace;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import genex.iterators.distance;
import genex.iterators.iter_pair;
import genex.operations.cmp;
import std;


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    concept replaceable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, std::projected<I, Proj>, std::remove_cvref_t<Old> const*> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename Rng, typename Old, typename New, typename Proj>
    concept replaceable_range =
        input_range<Rng> and
        replaceable_iters<iterator_t<Rng>, sentinel_t<Rng>, Old, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    struct replace_iterator {
        I it; S st;
        GENEX_NO_UNIQUE_ADDRESS Old old_value;
        GENEX_NO_UNIQUE_ADDRESS New new_value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        using value_type = iter_value_t<I>;
        using reference_type = iter_reference_t<I>;
        using difference_type = iter_difference_t<I>;
        using iterator_category = std::iterator_traits<I>::iterator_category;
        using iterator_concept = iterator_category;
        GENEX_ITER_OPS(replace_iterator)

        GENEX_INLINE constexpr replace_iterator() = default;

        GENEX_INLINE constexpr replace_iterator(I first, S last, Old old_val, New new_val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            old_value(std::move(old_val)), new_value(std::move(new_val)),
            proj(std::move(proj)) {
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
            return operations::eq{}(meta::invoke(self.proj, *self.it), self.old_value) ? *&self.new_value : *self.it;
        }

        GENEX_VIEW_ITER_EQ(replace_iterator, replace_iterator) {
            return self.it == that.it;
        }
    };


    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    struct replace_view {
        I it;
        S st;
        GENEX_NO_UNIQUE_ADDRESS Old old_value;
        GENEX_NO_UNIQUE_ADDRESS New new_value;
        GENEX_NO_UNIQUE_ADDRESS Proj proj;

        GENEX_INLINE constexpr replace_view(I first, S last, Old old_val, New new_val, Proj proj) :
            it(std::move(first)), st(std::move(last)),
            old_value(std::move(old_val)), new_value(std::move(new_val)), proj(std::move(proj)) {
        }

        template <typename Self>
        GENEX_ITER_BEGIN {
            return replace_iterator(self.it, self.st, self.old_value, self.new_value, self.proj);
        }

        template <typename Self>
        GENEX_ITER_END {
            return replace_iterator(self.st, self.st, self.old_value, self.new_value, self.proj);
        }

        template <typename Self>
        GENEX_ITER_SIZE {
            return iterators::distance(self.it, self.st);
        }
    };
}


namespace genex::views {
    struct replace_fn {
        template <typename I, typename S, typename Old, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_iters<I, S, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Old old_value, New new_value, Proj proj = {}) const {
            return detail::replace_view(std::move(first), std::move(last), std::move(old_value), std::move(new_value), std::move(proj));
        }

        template <typename Rng, typename Old, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_range<Rng, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Old old_value, New new_value, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::replace_view(std::move(first), std::move(last), std::move(old_value), std::move(new_value), std::move(proj));
        }

        template <typename Old, typename New, typename Proj = meta::identity>
        requires (not range<Old>)
        GENEX_INLINE constexpr auto operator()(Old old_value, New new_value, Proj proj = {}) const {
            return meta::bind_back(replace_fn{}, std::move(old_value), std::move(new_value), std::move(proj));
        }
    };

    export inline constexpr replace_fn replace{};
}
