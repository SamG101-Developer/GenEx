module;
#include <iterator>

export module genex.operations.size;
import genex.type_traits;

using namespace genex::type_traits;


namespace genex::operations {
    export template <typename Rng>
    concept has_member_size = requires(Rng &&r) { r.size(); };

    export template <typename Rng>
    concept has_std_size = requires(Rng &&r) { std::size(r); };

    struct size_fn {
        template <typename Rng> requires has_member_size<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> size_t {
            return r.size();
        }

        template <typename Rng> requires not has_member_size<Rng> and has_std_size<Rng>
        constexpr auto operator()(Rng &&r) const noexcept -> size_t {
            return std::size(r);
        }
    };

    export inline constexpr size_fn size;
}
