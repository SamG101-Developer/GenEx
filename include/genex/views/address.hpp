#pragma once
#include <coroutine>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <typename I, typename S>
    auto do_deref(I first, S last) -> generator<deref_value_t<iter_value_t<I>>> {
        for (auto it = first; it != last; ++it) {
            co_yield **it;
        }
    }

    template <typename I, typename S>
    auto do_address(I first, S last) -> generator<iter_value_t<I>*> {
        for (auto it = first; it != last; ++it) {
            co_yield &*it;
        }
    }
}


namespace genex::views {
    template <typename I, typename S>
    concept can_deref_iters =
        input_iterator<I> and
        sentinel_for<S, I>;

    template <typename Rng>
    concept can_deref_range =
        input_range<Rng>;

    template <typename I, typename S>
    concept can_address_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::is_lvalue_reference_v<iter_reference_t<I>>;

    template <typename Rng>
    concept can_address_range =
        input_range<Rng> and
        std::is_lvalue_reference_v<range_reference_t<Rng>>;

    DEFINE_VIEW(deref) {
        template <typename I, typename S> requires can_deref_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_deref(std::move(first), std::move(last));
            return deref_view(std::move(gen));
        }

        template <typename Rng> requires can_deref_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_deref_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_VIEW(address) {
        template <typename I, typename S> requires can_address_iters<I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_address(std::move(first), std::move(last));
            return address_view(std::move(gen));
        }

        template <typename Rng> requires can_address_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return (*this)(iterators::begin(rng), iterators::end(rng));
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_address_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(deref);
    EXPORT_GENEX_VIEW(address);
}
