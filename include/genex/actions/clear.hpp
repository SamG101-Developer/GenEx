#pragma once
#include <genex/concepts.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::detail::concepts {
    template <typename Rng>
    concept clearable_range =
        input_range<Rng> and
        has_member_clear<Rng>;
}


namespace genex::actions {
    struct clear_fn {
        template <typename Rng>
        requires detail::concepts::clearable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> decltype(auto) {
            rng.clear();
            return std::forward<Rng>(rng);
        }

        GENEX_INLINE constexpr auto operator()() const {
            return std::bind_back(clear_fn{});
        }
    };

    inline constexpr clear_fn clear{};
}
