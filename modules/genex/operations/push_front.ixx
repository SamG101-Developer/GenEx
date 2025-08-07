module;
#include <utility>
#include <genex/macros.hpp>

export module genex.operations.push_front;
import genex.operations.insert;
import genex.type_traits;

using namespace genex::type_traits;


namespace genex::operations {
    export template <typename Rng>
    concept has_member_push_front = requires(Rng &&r) { r.push_front(std::declval<range_value_t<Rng>>()); };

    struct push_front_fn {
        template <typename Rng> requires (has_member_push_front<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.push_front(std::forward<range_value_t<Rng>>(elem));
        }

        template <typename Rng> requires (not has_member_push_front<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem), 0);
        }
    };

    EXPORT_GENEX_STRUCT(push_front);
}
