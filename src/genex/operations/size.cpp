module;
#include <genex/macros.hpp>

export module genex.operations.size;
import genex.concepts;
import std;


namespace genex::operations::detail::concepts {
    template <typename Rng>
    concept sizeable_range =
        range<Rng>;

    template <typename Rng>
    concept sizeable_select_size =
        sizeable_range<Rng> and
        has_member_size<Rng>;

    template <typename Rng>
    concept sizeable_select_std_size =
        sizeable_range<Rng> and
        has_std_size<Rng> and
        not has_member_size<Rng>;

    template <typename Rng>
    concept sizeable_select_else =
        sizeable_range<Rng> and
        not has_std_size<Rng> and
        not has_member_size<Rng>;
}


namespace genex::operations {
    struct size_fn {
        template <typename Rng>
        requires detail::concepts::sizeable_select_size<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&r) const -> std::size_t {
            return r.size();
        }

        template <typename Rng>
        requires detail::concepts::sizeable_select_std_size<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&r) const -> std::size_t {
            return std::size(r);
        }

        template <typename Rng>
        requires detail::concepts::sizeable_select_else<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&gen) const -> std::size_t {
            auto count = static_cast<std::size_t>(0);
            for (auto &&_ : gen) { ++count; }
            return count;
        }
    };

    export inline constexpr size_fn size{};
}
