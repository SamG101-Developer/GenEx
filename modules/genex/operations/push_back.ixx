module;
#include <utility>
#include <genex/macros.hpp>

export module genex.operations.push_back;
import genex.operations.size;
import genex.operations.insert;
import genex.type_traits;

using namespace genex::type_traits;


namespace genex::operations {
    export template <typename Rng>
    concept has_member_push_back = requires(Rng &&r) { r.push_back(std::declval<range_value_t<Rng>>()); };

    struct push_back_fn {
        template <typename Rng> requires (has_member_push_back<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.push_back(std::forward<range_value_t<Rng>>(elem));
        }

        template <typename Rng> requires (not has_member_push_back<Rng> and has_member_insert<Rng>)
        constexpr auto operator()(Rng &&r, range_value_t<Rng> const &elem) const noexcept -> void {
            r.insert(std::forward<range_value_t<Rng>>(elem), operations::size(r));
        }
    };

    EXPORT_GENEX_STRUCT(push_back);
}
