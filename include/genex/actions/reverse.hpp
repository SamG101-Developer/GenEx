#pragma once
#include <utility>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::actions {
    template <typename Rng>
    concept can_reverse_range =
        bidirectional_range<Rng> and
        std::permutable<iterator_t<Rng>>;

    DEFINE_ACTION(reverse) {
        template <typename Rng> requires can_reverse_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> Rng& {
            std::reverse(iterators::begin(rng), iterators::end(rng));
            return rng;
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires can_reverse_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_ACTION(reverse);
}
