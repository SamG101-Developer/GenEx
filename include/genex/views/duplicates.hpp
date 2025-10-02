#pragma once
#include <coroutine>
#include <functional>
#include <optional>
#include <vector>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/operations/cmp.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept duplicate_checkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirectly_comparable<I, I, Comp, Proj, meta::identity>;

    template <typename Rng, typename Comp, typename Proj>
    concept duplicate_checkable_range =
        forward_range<Rng> and
        duplicate_checkable_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Comp, typename Proj>
        requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
    GENEX_NO_ASAN
    auto do_duplicates(I first, S last, Comp &&comp, Proj &&proj) -> generator<iter_value_t<I>> {
        if (first == last) { co_return; }

        auto seen = std::vector<iter_value_t<I>>{};
        auto dupe_element = std::optional<iter_value_t<I>>{};

        for (; first != last; ++first) {
            auto cur = std::invoke(std::forward<Proj>(proj), *first);
            if (dupe_element.has_value()) {
                if (std::invoke(std::forward<Comp>(comp), cur, std::invoke(std::forward<Proj>(proj), *dupe_element))) {
                    co_yield static_cast<iter_value_t<I>>(*first);
                }
                continue;
            }

            for (const auto &s : seen) {
                if (std::invoke(std::forward<Comp>(comp), cur, std::invoke(std::forward<Proj>(proj), s))) {
                    dupe_element = cur;
                    co_yield static_cast<iter_value_t<I>>(*dupe_element);
                    co_yield static_cast<iter_value_t<I>>(*first);
                    break;
                }
            }
            seen.emplace_back(*first);
        }
    }
}


namespace genex::views {
    struct duplicates_fn {
        template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity>
            requires concepts::duplicate_checkable_iters<I, S, Comp, Proj>
        constexpr auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return detail::do_duplicates(
                std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity>
            requires concepts::duplicate_checkable_range<Rng, Comp, Proj>
        constexpr auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Comp = operations::eq, typename Proj = meta::identity>
            requires (not range<Comp>)
        constexpr auto operator()(Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return std::bind_back(
                duplicates_fn{}, std::forward<Comp>(comp), std::forward<Proj>(proj));
        }
    };

    GENEX_EXPORT_STRUCT(duplicates);
}
