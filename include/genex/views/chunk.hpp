#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/operations/cmp.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S, typename E>
    concept chunkable_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_equivalence_relation<operations::eq, I, std::remove_cvref_t<E> const*>;;

    template <typename Rng, typename E>
    concept chunkable_range =
        forward_range<Rng> and
        chunkable_iters<iterator_t<Rng>, sentinel_t<Rng>, E>;
}


namespace genex::views::detail {
    template <typename I, typename S, typename E>
        requires concepts::chunkable_iters<I, S, E>
    auto do_chunk(I first, S last, E &&val) -> generator<std::vector<iter_value_t<I>>> {
        while (first != last) {
            auto chunk = std::vector<iter_value_t<I>>{};
            for (; first != last and val != *first; ++first) {
                chunk.push_back(*first);
            }
            co_yield std::move(chunk);
            if (first != last) { ++first; }
        }
    }
}


namespace genex::views {
    struct chunk_fn {
        template <typename I, typename S, typename E>
            requires concepts::chunkable_iters<I, S, E>
        constexpr auto operator()(I first, S last, E &&val) const -> auto {
            return detail::do_chunk(
                std::move(first), std::move(last), std::forward<E>(val));
        }

        template <typename Rng, typename E>
            requires concepts::chunkable_range<Rng, E>
        constexpr auto operator()(Rng &&rng, E &&val) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(
                std::move(first), std::move(last), std::forward<E>(val));
        }

        template <typename E>
        constexpr auto operator()(E &&val) const -> auto {
            return std::bind_back(chunk_fn{}, std::forward<E>(val));
        }
    };

    GENEX_EXPORT_STRUCT(chunk);
}
