#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>
BROKEN("Chunk view is broken")


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_chunk_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I>;

    template <typename Rng>
    concept can_chunk_range =
        forward_range<Rng> and
        can_chunk_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_chunk_iters<I, S>
    auto do_chunk(I first, S last, const std::size_t size) -> generator<generator<iter_value_t<I>>> {
        while (first != last) {
            auto chunk_start = first;
            auto chunk_size = std::min(size, static_cast<std::size_t>(std::distance(first, last)));
            auto chunk_end = std::next(first, chunk_size);

            auto chunk = [chunk_start, chunk_end]() -> generator<iter_value_t<I>> {
                for (auto it = chunk_start; it != chunk_end; ++it) {
                    co_yield *it;
                }
            }();

            first = chunk_end;
            co_yield std::move(chunk);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(chunk) {
        template <typename I, typename S> requires concepts::can_chunk_iters<I, S>
        auto operator()(I first, S last, size_t size) const -> auto {
            auto gen = detail::do_chunk(std::move(first), std::move(last), size);
            return chunk_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_chunk_range<Rng>
        auto operator()(Rng &&rng, size_t size) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), size);
        }

        auto operator()(std::size_t n) const -> auto {
            return
                [FWD_CAPTURES(n)]<typename Rng> requires concepts::can_chunk_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    EXPORT_GENEX_VIEW(chunk);
}
