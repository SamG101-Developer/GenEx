#pragma once
#include <coroutine>
#include <functional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Old, typename New, typename Proj>
    concept can_replace_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_comparable<I, const std::remove_cvref_t<Old>*, operations::eq, Proj, meta::identity> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename I, typename S, typename Pred, typename New, typename Proj>
    concept can_replace_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>> and
        std::convertible_to<New, iter_value_t<I>>;

    template <typename Rng, typename Old, typename New, typename Proj>
    concept can_replace_range =
        input_range<Rng> and
        can_replace_iters<iterator_t<Rng>, sentinel_t<Rng>, Old, New, Proj>;

    template <typename Rng, typename Pred, typename New, typename Proj>
    concept can_replace_if_range =
        input_range<Rng> and
        can_replace_if_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, New, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Old, typename New, typename Proj> requires concepts::can_replace_iters<I, S, Old, New, Proj>
    auto do_replace(I first, S last, Old &&old_val, New &&new_val, Proj &&proj) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (std::invoke(proj, *first) == old_val) { co_yield new_val; }
            else { co_yield static_cast<iter_value_t<I>>(*first); }
        }
    }

    template <typename I, typename S, typename Pred, typename New, typename Proj> requires concepts::can_replace_if_iters<I, S, Pred, New, Proj>
    auto do_replace_if(I first, S last, Pred &&pred, New &&new_val, Proj &&proj) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            if (std::invoke(pred, std::invoke(proj, *first))) { co_yield new_val; }
            else { co_yield static_cast<iter_value_t<I>>(*first); }
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(replace) {
        template <typename I, typename S, typename Old, typename New, typename Proj = meta::identity> requires concepts::can_replace_iters<I, S, Old, New, Proj>
        auto operator()(I first, S last, Old &&old_val, New &&new_val, Proj &&proj = {}) const -> auto {
            // Call the replace inner function.
            auto gen = detail::do_replace(std::move(first), std::move(last), std::forward<Old>(old_val), std::forward<New>(new_val), std::forward<Proj>(proj));
            return replace_view(std::move(gen));
        }

        template <typename Rng, typename Old, typename New, typename Proj = meta::identity> requires concepts::can_replace_range<Rng, Old, New, Proj>
        auto operator()(Rng &&rng, Old &&old_val, New &&new_val, Proj &&proj = {}) const -> auto {
            // Call the replace inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Old>(old_val), std::forward<New>(new_val), std::forward<Proj>(proj));
        }

        template <typename Old, typename New, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Old>>)
        auto operator()(Old &&old_val, New &&new_val, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the replace.
            return
                [FWD_CAPTURES(old_val, new_val, proj)]<typename Rng> requires concepts::can_replace_range<Rng, Old, New, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(old_val), std::move(new_val), std::move(proj));
            };
        }
    };

    DEFINE_VIEW(replace_if) {
        template <typename I, typename S, typename Pred, typename New, typename Proj = meta::identity> requires concepts::can_replace_if_iters<I, S, Pred, New, Proj>
        auto operator()(I first, S last, Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            // Call the replace_if inner function.
            auto gen = detail::do_replace_if(std::move(first), std::move(last), std::forward<Pred>(pred), std::forward<New>(new_val), std::forward<Proj>(proj));
            return replace_if_view(std::move(gen));
        }

        template <typename Rng, typename Pred, typename New, typename Proj = meta::identity> requires concepts::can_replace_if_range<Rng, Pred, New, Proj>
        auto operator()(Rng &&rng, Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            // Call the replace_if inner function.
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Pred>(pred), std::forward<New>(new_val), std::forward<Proj>(proj));
        }

        template <typename Pred, typename New, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Pred>>)
        auto operator()(Pred &&pred, New &&new_val, Proj &&proj = {}) const -> auto {
            // Create a closure that takes a range and applies the replace_if.
            return
                [FWD_CAPTURES(pred, new_val, proj)]<typename Rng> requires concepts::can_replace_if_range<Rng, Pred, New, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::move(pred), std::move(new_val), std::move(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(replace);
    EXPORT_GENEX_VIEW(replace_if);
}
