#pragma once
#include <functional>
#include <random>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::actions::detail {
    template <range Rng, typename Shuffler>
    auto do_shuffle(Rng *rng, Shuffler &&shuffler) -> void {
        std::shuffle(iterators::begin(*rng), iterators::end(*rng), std::forward<Shuffler>(shuffler));
    }

    template <range Rng>
    auto do_random_shuffle(Rng *rng) -> void {
        auto rd = std::random_device{};
        auto shuffler = std::mt19937{rd()};
        std::shuffle(iterators::begin(*rng), iterators::end(*rng), std::move(shuffler));
    }
}


namespace genex::actions {
    DEFINE_ACTION(shuffle) {
        template <range Rng, typename Shuffler>
        constexpr auto operator()(Rng &&rng, Shuffler &&shuffler) const -> auto {
            FWD_TO_IMPL(detail::do_shuffle, &rng, shuffler);
        }

        template <typename Shuffler>
        constexpr auto operator()(Shuffler &&shuffler) const -> auto {
            MAKE_CLOSURE(shuffler);
        }
    };

    DEFINE_ACTION(random_shuffle) {
        template <range Rng>
        constexpr auto operator()(Rng &&rng) const -> auto {
            FWD_TO_IMPL(detail::do_shuffle, &rng);
        }

        constexpr auto operator()() const -> auto {
            MAKE_CLOSURE();
        }
    };

    EXPORT_GENEX_ACTION(shuffle);
    EXPORT_GENEX_ACTION(random_shuffle);
}
