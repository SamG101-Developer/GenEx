module;
#include <genex/macros.hpp>

export module genex.operations.empty;
import genex.concepts;
import genex.operations.size;


namespace genex::operations::detail::concepts {
    template <typename Rng>
    concept emptyable_range =
        range<Rng>;

    template <typename Rng>
    concept emptyable_select_empty =
        emptyable_range<Rng> and
        has_member_empty<Rng>;

    template <typename Rng>
    concept emptyable_select_size =
        emptyable_range<Rng> and
        not has_member_empty<Rng>;
}


namespace genex::operations {
    struct empty_fn {
        template <typename Rng>
        requires detail::concepts::emptyable_select_empty<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&r) const -> bool {
            return r.empty();
        }

        template <typename Rng>
        requires detail::concepts::emptyable_select_size<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const -> bool {
            return operations::size(rng) == 0;
        }
    };

    export inline constexpr empty_fn empty{};
}
