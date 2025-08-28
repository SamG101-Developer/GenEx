#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/iterators/next.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

// todo: this doesn't work


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_chunk(I first, S last, std::size_t size) -> generator<generator<iter_value_t<I>>> {
        for (; first != last; first += size) {
            co_yield [chunk_start = first, chunk_end = iterators::next(first, size), last] mutable -> generator<iter_value_t<I>> {
                for (; chunk_start != chunk_end and chunk_start != last; ++chunk_start) {
                    co_yield *chunk_start;
                }
            }();
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_chunk_iters =
        std::forward_iterator<I> and
        std::sentinel_for<S, I>;

    template <typename Rng>
    concept can_chunk_range =
        forward_range<Rng>;

    DEFINE_VIEW(chunk) {
        template <typename I, typename S> requires can_chunk_iters<I, S>
        constexpr auto operator()(I first, S last, size_t size) const -> auto {
            auto gen = detail::do_chunk(std::move(first), std::move(last), size);
            return chunk_view(std::move(gen));
        }

        template <typename Rng> requires can_chunk_range<Rng>
        constexpr auto operator()(Rng &&rng, size_t size) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng), size);
        }

        constexpr auto operator()(std::size_t n) const -> auto {
            return
                [FWD_CAPTURES(n)]<typename Rng> requires can_chunk_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), n);
            };
        }
    };

    EXPORT_GENEX_VIEW(chunk);
}
