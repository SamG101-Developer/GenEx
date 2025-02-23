module;
#include <utility>

export module genex.operations.insert;
import genex.type_traits;

using namespace genex::type_traits;


namespace genex::operations {
    export template <typename Rng>
    concept has_member_insert = requires(Rng &&r) { r.insert(std::declval<range_value_t<Rng>>(), std::declval<size_t>()); };

    struct insert_fn {
        template <typename Rng> requires has_member_insert<Rng>
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem));
        }
    };

    export inline constexpr insert_fn insert;
}
