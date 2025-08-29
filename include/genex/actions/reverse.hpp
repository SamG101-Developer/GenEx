#pragma once
#include <utility>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::actions::concepts {
    template <typename Rng>
    concept can_reverse_range =
        bidirectional_range<Rng> and
        std::permutable<iterator_t<Rng>>;
}


namespace genex::actions {
    DEFINE_ACTION(reverse) {
        template <typename Rng> requires concepts::can_reverse_range<Rng>
        auto operator()(Rng &&rng) const -> void {
            std::reverse(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_reverse_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_ACTION(reverse);
}
