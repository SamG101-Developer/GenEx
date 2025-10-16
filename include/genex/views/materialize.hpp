#pragma once
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/actions/push_back.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <template <typename> typename Cache, typename I, typename S>
    concept materializable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        // std::is_lvalue_reference_v<iter_reference_t<I>> and
        actions::detail::concepts::back_insertable_range<Cache<iter_value_t<I>>, iter_value_t<I>> and
        std::is_constructible_v<Cache<iter_value_t<I>>>;

    template <template <typename> typename Cache, typename Rng>
    concept materializable_range =
        input_range<Rng> and
        materializable_iters<Cache, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail::impl {
    template <template <typename> typename Cache, typename I, typename S>
    requires concepts::materializable_iters<Cache, I, S>
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
    template <template <typename> typename Cache>
    struct materialize_fn {
        template <typename I, typename S>
        requires detail::concepts::materializable_iters<Cache, I, S>
        GENEX_INLINE constexpr auto operator()(I first, S last) const {
            return detail::impl::do_materialize<Cache>(std::move(first), std::move(last));
        }

        template <typename Rng>
        requires detail::concepts::materializable_range<Cache, Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const {
            auto [first, last] = iterators::iter_pair(rng);
            return detail::impl::do_materialize<Cache>(std::move(first), std::move(last));
        }

        GENEX_INLINE constexpr auto operator()() const {
            return meta::bind_back(materialize_fn{});
        }
    };

    inline constexpr materialize_fn<std::vector> materialize{};
}

