#pragma once
#include <functional>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::concepts {
    template <typename Rng>
    concept clearable_range =
        input_range<Rng> and
        has_member_clear<Rng>;
}


namespace genex::actions {
    struct clear_fn {
        template <typename Rng>
        requires concepts::clearable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            rng.clear();
            return std::forward<Rng>(rng);
        }

        GENEX_INLINE constexpr auto operator()() const -> auto {
            return std::bind_back(clear_fn{});
        }
    };

    GENEX_EXPORT_STRUCT(clear);
}
