#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <typename I, typename S>
    concept can_deref_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_readable<I> and
        requires(I it) { { **it }; };

    template <typename I, typename S>
    concept can_address_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        std::indirectly_readable<I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        requires(I it) { { std::addressof(*it) }; };

    template <typename Rng>
    concept can_deref_range =
        input_range<Rng> and
        can_deref_iters<iterator_t<Rng>, sentinel_t<Rng>>;

    template <typename Rng>
    concept can_address_range =
        input_range<Rng> and
        can_address_iters<iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <typename I, typename S> requires concepts::can_deref_iters<I, S>
    auto do_deref(I first, S last) -> generator<decltype(**std::declval<I&>())> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield **first;
        }
    }

    template <typename I, typename S> requires concepts::can_address_iters<I, S>
    auto do_address(I first, S last) -> generator<std::add_pointer_t<std::remove_reference_t<decltype(*std::declval<I&>())>>> {
        if (first == last) { co_return; }
        for (; first != last; ++first) {
            co_yield std::addressof(*first);
        }
    }
}


namespace genex::views {
    DEFINE_VIEW(deref) {
        template <typename I, typename S> requires concepts::can_deref_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_deref(std::move(first), std::move(last));
            return deref_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_deref_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_deref_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(address) {
        template <typename I, typename S> requires concepts::can_address_iters<I, S>
        auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_address(std::move(first), std::move(last));
            return address_view(std::move(gen));
        }

        template <typename Rng> requires concepts::can_address_range<Rng>
        auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_address_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(deref);
    EXPORT_GENEX_VIEW(address);
}
