#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


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


namespace genex::views::detail::impl {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    requires concepts::replaceable_iters<I, S, Old, New, Proj>
    auto do_replace(I first, S last, Old old_val, New new_val, Proj proj) -> generator<iter_value_t<I>> {
        GENEX_ITER_GUARD;
        for (; first != last; ++first) {
            if (meta::invoke(proj, *first) == old_val) { co_yield new_val; }
            else { co_yield *first; }
        }
    }
}


namespace genex::views {
    struct replace_fn {
        template <typename I, typename S, typename Old, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_iters<I, S, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(I first, S last, Old old_val, New new_val, Proj proj = {}) const {
            return detail::impl::do_replace(std::move(first), std::move(last), std::move(old_val), std::move(new_val), std::move(proj));
        }

        template <typename Rng, typename Old, typename New, typename Proj = meta::identity>
        requires detail::concepts::replaceable_range<Rng, Old, New, Proj>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Old old_val, New new_val, Proj proj = {}) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_replace(std::move(first), std::move(last), std::move(old_val), std::move(new_val), std::move(proj));
        }

        template <typename Old, typename New, typename Proj = meta::identity>
        requires (not range<Proj>)
        GENEX_INLINE constexpr auto operator()(Old old_val, New new_val, Proj proj = {}) const {
            return meta::bind_back(replace_fn{}, std::move(old_val), std::move(new_val), std::move(proj));
        }
    };

    inline constexpr replace_fn replace{};
}
