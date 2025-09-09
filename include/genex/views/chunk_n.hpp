#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept chunkable_n_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept chunkable_n_range =
        forward_range<Rng> and
        chunkable_n_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S>
        requires concepts::chunkable_n_iters<I, S>
    auto do_chunk_n(I first, S last, const std::size_t size) -> generator<std::vector<iter_value_t<I>>> {
        while (first != last) {
            auto chunk = std::vector<iter_value_t<I>>{};
            for (auto i = 0uz; i < size && first != last; ++i, ++first) {
                chunk.push_back(*first);
            }
            co_yield std::move(chunk);
        }
    }
}


namespace genex::views {
    struct chunk_n_fn {
        template <typename I, typename S>
            requires concepts::chunkable_n_iters<I, S>
        constexpr auto operator()(I first, S last, size_t size) const -> auto {
            return detail::do_chunk_n(std::move(first), std::move(last), size);
        }

        template <typename Rng>
            requires concepts::chunkable_n_range<Rng>
        constexpr auto operator()(Rng &&rng, size_t size) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return (*this)(std::move(first), std::move(last), size);
        }

        constexpr auto operator()(std::size_t n) const -> auto {
            return std::bind_back(chunk_n_fn{}, n);
        }
    };

    GENEX_EXPORT_STRUCT(chunk_n);
}
