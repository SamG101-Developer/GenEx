#pragma once
#include <coroutine>
#include <vector>
#include <genex/concepts.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>


namespace genex::views::detail {
    template <template <typename> typename Cache, typename I, typename S>
    auto do_materialize(I first, S last) -> Cache<std::reference_wrapper<iter_value_t<I>>> {
        auto vec = Cache<std::reference_wrapper<iter_value_t<I>>>();
        for (; first != last; ++first) {
            auto &&temp = *first;
            vec.push_back(std::ref(temp));
        }
        return vec;
    }
}


namespace genex::views {
    template <template <typename> typename Cache, typename I, typename S>
    concept can_materialize_iters =
        input_iterator<I> and
        sentinel_for<S, I> and
        std::is_lvalue_reference_v<iter_reference_t<I>> and
        requires { Cache<std::reference_wrapper<iter_value_t<I>>>::push_back(std::ref(*std::declval<I>())); };

    template <template <typename> typename Cache, typename Rng>
    concept can_materialize_range =
        input_range<Rng> and
        std::is_lvalue_reference_v<range_reference_t<Rng>> and
        requires { Cache<std::reference_wrapper<range_value_t<Rng>>>::push_back(std::ref(*iterators::begin(std::declval<Rng>()))); };

    DEFINE_VIEW(materialize) {
        template <template <typename> typename Cache = std::vector, typename I, typename S>
        constexpr auto operator()(I first, S last) const -> auto {
            auto gen = detail::do_materialize<Cache>(std::move(first), std::move(last));
            return gen;
        }

        template <template <typename> typename Cache = std::vector, typename Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            return this->operator()<Cache>(iterators::begin(rng), iterators::end(rng));
        }

        template <template <typename> typename Cache = std::vector>
        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_materialize_range<Cache, Rng>
                (Rng &&rng) mutable -> auto {
                return this->operator()<Cache>(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_VIEW(materialize);
}
