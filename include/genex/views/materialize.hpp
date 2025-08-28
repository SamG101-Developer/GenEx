#pragma once
#include <coroutine>
#include <vector>
#include <genex/actions/push.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::concepts {
    template <template <typename> typename Cache, typename I, typename S>
    concept can_materialize_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::movable<I> and
        // std::is_lvalue_reference_v<iter_reference_t<I>> and
        actions::concepts::can_push_back_range<Cache<iter_value_t<I>>, iter_value_t<I>> and
        std::is_constructible_v<Cache<iter_value_t<I>>>;

    template <template <typename> typename Cache, typename Rng>
    concept can_materialize_range =
        input_range<Rng> and
        can_materialize_iters<Cache, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <template <typename> typename Cache, typename I, typename S> requires concepts::can_materialize_iters<Cache, I, S>
    auto do_materialize(I first, S last) -> Cache<iter_value_t<I>> {
        auto vec = Cache<iter_value_t<I>>();
        if (first == last) { return vec; }
        for (; first != last; ++first) {
            vec |= actions::push_back(*first);
        }
        return vec;
    }
}


namespace genex::views {
    DEFINE_VIEW(materialize) {
        template <template <typename> typename Cache = std::vector, typename I, typename S> requires concepts::can_materialize_iters<Cache, I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_materialize<Cache>(std::move(first), std::move(last));
            return materialize_view(std::move(gen));
        }

        template <template <typename> typename Cache = std::vector, typename Rng> requires concepts::can_materialize_range<Cache, Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return this->operator()<Cache>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        template <template <typename> typename Cache = std::vector>
        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_materialize_range<Cache, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<Cache>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(materialize);
}
