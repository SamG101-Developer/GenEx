module;
#include <genex/macros.hpp>

export module genex.actions.clear;
export import genex.pipe;
import genex.meta;
import genex.concepts;
import std;


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
            return meta::bind_back(clear_fn{});
        }
    };

    export inline constexpr clear_fn clear{};
}
