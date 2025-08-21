#pragma once
#include <coroutine>
#include <functional>
#include <vector>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/views/_view_base.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views::detail {
    template <iterator I, sentinel S>
    auto do_materialize(I &&first, S &&last) -> std::vector<iter_value_t<I>> {
        auto vec = std::vector<iter_value_t<I>>();
        for (; first != last; ++first) {
            vec.push_back(std::forward<iter_value_t<I>>(*first));
        }
        return vec;
    }

    template <range Rng>
    auto do_materialize(Rng &&rng) -> std::vector<range_value_t<Rng>> {
        auto vec = std::vector<range_value_t<Rng>>();
        for (auto &&x : rng) {
            vec.push_back(std::forward<range_value_t<Rng>>(x));
        }
        return vec;
    }
}


namespace genex::views {
    DEFINE_VIEW(materialize) {
        DEFINE_OUTPUT_TYPE(materialize);

        template <iterator I, sentinel S>
        constexpr auto operator()(I &&first, S &&last) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_materialize, first, last);
        }

        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL_VIEW(detail::do_materialize, rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_VIEW(materialize);
}
