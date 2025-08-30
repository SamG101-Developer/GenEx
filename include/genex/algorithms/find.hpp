#pragma once
#include <functional>
#include <genex/algorithms/_algorithm_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::algorithms::concepts {
    template <typename I, typename S, typename E, typename Proj>
    concept can_find_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_unary_invocable<Proj, I> and
        std::indirectly_comparable<I, const std::remove_cvref_t<E>*, operations::eq, Proj, meta::identity>;

    template <typename I, typename S, typename E, typename Proj>
    concept can_find_last_iters_optimized =
        std::bidirectional_iterator<I> and
        can_find_iters<I, S, E, Proj>;

    template <typename I, typename S, typename Pred, typename Proj>
    concept can_find_if_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    template <typename I, typename S, typename E, typename Proj>
    concept can_find_if_last_iters_optimized =
        can_find_last_iters_optimized<I, S, E, Proj> and
        can_find_if_iters<I, S, E, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept can_find_range =
        input_range<Rng> and
        can_find_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept can_find_last_range_optimized =
        bidirectional_range<Rng> and
        can_find_range<Rng, E, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept can_find_if_range =
        input_range<Rng> and
        can_find_if_iters<iterator_t<Rng>, sentinel_t<Rng>, E, Proj>;

    template <typename Rng, typename E, typename Proj>
    concept can_find_if_last_range_optimized =
        can_find_last_range_optimized<Rng, E, Proj> and
        can_find_if_range<Rng, E, Proj>;
}


namespace genex::algorithms {
    DEFINE_ALGORITHM(find) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires concepts::can_find_iters<I, S, E, Proj>
        auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == elem) { return first; }
            }
            return last;
        }

        template <typename Rng, typename E, typename Proj = meta::identity> requires concepts::can_find_range<Rng, E, Proj>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    DEFINE_ALGORITHM(find_last) {
        template <typename I, typename S, typename E, typename Proj = meta::identity> requires (concepts::can_find_iters<I, S, E, Proj> and not concepts::can_find_last_iters_optimized<I, S, E, Proj>)
        auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto found_last = last;
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Proj>(proj), *first) == elem) { found_last = first; }
            }
            return found_last;
        }

        template <typename I, typename S, typename E, typename Proj = meta::identity> requires concepts::can_find_last_iters_optimized<I, S, E, Proj>
        auto operator()(I first, S last, E &&elem, Proj &&proj = {}) const -> auto {
            auto true_last = last;
            while (last != first) {
                --last;
                if (std::invoke(std::forward<Proj>(proj), *last) == elem) { return last; }
            }
            return true_last;
        }

        template <typename Rng, typename E, typename Proj = meta::identity> requires concepts::can_find_range<Rng, E, Proj>
        auto operator()(Rng &&rng, E &&elem, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<E>(elem), std::forward<Proj>(proj));
        }
    };

    DEFINE_ALGORITHM(find_if) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_iters<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { return first; }
            }
            return last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    DEFINE_ALGORITHM(find_last_if) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires (concepts::can_find_if_iters<I, S, Pred, Proj> and not concepts::can_find_if_last_iters_optimized<I, S, Pred, Proj>)
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto found_last = last;
            for (; first != last; ++first) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { found_last = first; }
            }
            return found_last;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_last_iters_optimized<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto true_last = last;
            for (; last != first; --last) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) { return last; }
            }
            return true_last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };


    DEFINE_ALGORITHM(find_if_not) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            for (; first != last; ++first) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { return first; }
            }
            return last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    DEFINE_ALGORITHM(find_last_if_not) {
        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires (concepts::can_find_if_iters<I, S, Pred, Proj> and not concepts::can_find_if_last_iters_optimized<I, S, Pred, Proj>)
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto found_last = last;
            for (; first != last; ++first) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *first))) { found_last = first; }
            }
            return found_last;
        }

        template <typename I, typename S, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_last_iters_optimized<I, S, Pred, Proj>
        auto operator()(I first, S last, Pred &&pred, Proj &&proj = {}) const -> auto {
            auto true_last = last;
            for (; last != first; --last) {
                if (not std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *last))) { return last; }
            }
            return true_last;
        }

        template <typename Rng, typename Pred, typename Proj = meta::identity> requires concepts::can_find_if_range<Rng, Pred, Proj>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    EXPORT_GENEX_ALGORITHM(find);
    EXPORT_GENEX_ALGORITHM(find_last);
    EXPORT_GENEX_ALGORITHM(find_if);
    EXPORT_GENEX_ALGORITHM(find_last_if);
    EXPORT_GENEX_ALGORITHM(find_if_not);
    EXPORT_GENEX_ALGORITHM(find_last_if_not);
}
