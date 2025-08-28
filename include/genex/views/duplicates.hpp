#pragma once
#include <coroutine>
#include <vector>
#include <functional>
#include <optional>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/meta.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename Comp, typename Proj>
    concept can_duplicates_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_comparable<I, I, Comp, Proj, meta::identity>;

    template <typename Rng, typename Comp, typename Proj>
    concept can_duplicates_range =
        forward_range<Rng> and
        can_duplicates_iters<iterator_t<Rng>, sentinel_t<Rng>, Comp, Proj>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename Comp, typename Proj> requires concepts::can_duplicates_iters<I, S, Comp, Proj>
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
            seen.push_back(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(duplicates) {
        template <typename I, typename S, typename Comp = operations::eq, typename Proj = meta::identity> requires concepts::can_duplicates_iters<I, S, Comp, Proj>
        auto operator()(I first, S last, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            auto gen = detail::do_duplicates(std::move(first), std::move(last), std::forward<Comp>(comp), std::forward<Proj>(proj));
            return duplicates_view(std::move(gen));
        }

        template <typename Rng, typename Comp = operations::eq, typename Proj = meta::identity> requires concepts::can_duplicates_range<Rng, Comp, Proj>
        auto operator()(Rng &&rng, Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Comp>(comp), std::forward<Proj>(proj));
        }

        template <typename Comp = operations::eq, typename Proj = meta::identity> requires (not input_range<std::remove_cvref_t<Comp>>)
        auto operator()(Comp &&comp = {}, Proj &&proj = {}) const -> auto {
            return
                [FWD_CAPTURES(comp, proj)]<typename Rng> requires concepts::can_duplicates_range<Rng, Comp, Proj>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Comp>(comp), std::forward<Proj>(proj));
            };
        }
    };

    EXPORT_GENEX_VIEW(duplicates);
}
