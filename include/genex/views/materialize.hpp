#pragma once
#include <coroutine>
#include <vector>
#include <genex/actions/push_back.hpp>
#include <genex/concepts.hpp>
#include <genex/iterators/iter_pair.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::views::concepts {
    template <template <typename> typename Cache, typename I, typename S>
    concept materializable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        // std::is_lvalue_reference_v<iter_reference_t<I>> and
        actions::concepts::back_insertable_range<Cache<iter_value_t<I>>, iter_value_t<I>> and
        std::is_constructible_v<Cache<iter_value_t<I>>>;

    template <template <typename> typename Cache, typename Rng>
    concept materializable_range =
        input_range<Rng> and
        materializable_iters<Cache, iterator_t<Rng>, sentinel_t<Rng>>;
}


namespace genex::views::detail {
    template <template <typename> typename Cache, typename I, typename S>
        requires concepts::materializable_iters<Cache, I, S>
    GENEX_NO_ASAN
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
    struct materialize_impl_fn {
        template <template <typename> typename Cache = std::vector, typename I, typename S>
            requires concepts::materializable_iters<Cache, I, S>
        constexpr auto operator()(I first, S last) const -> auto {
            return detail::do_materialize<Cache>(
                std::move(first), std::move(last));
        }

        template <template <typename> typename Cache = std::vector, typename Rng>
            requires concepts::materializable_range<Cache, Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            auto [first, last] = iterators::iter_pair(rng);
            return this->operator()<Cache>(
                std::move(first), std::move(last));
        }

        template <template <typename> typename Cache = std::vector>
        constexpr auto operator()() const -> auto {
            return [*this]<typename Rng>requires concepts::materializable_range<Cache, Rng>(Rng &&rng) {
                return this->operator()<Cache>(std::move(rng));
            };
        }
    };

    GENEX_EXPORT_STRUCT(materialize_impl);
}


#define materialize materialize_impl.operator()
