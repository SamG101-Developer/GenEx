#pragma once
#include <iterator>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


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
        sizeable_range<Rng> and
        not has_member_empty<Rng>;
}


namespace genex::operations {
    template <typename Rng>
    concept can_size_range =
        input_range<Rng> and
        std::sized_sentinel_for<sentinel_t<Rng>, iterator_t<Rng>>;

    struct size_fn {
        template <typename Rng>
            requires detail::concepts::sizeable_select_size<Rng>
        constexpr auto operator()(Rng &&r) const -> std::size_t {
            return r.size();
        }

        template <typename Rng>
            requires detail::concepts::sizeable_select_std_size<Rng>
        constexpr auto operator()(Rng &&r) const -> std::size_t {
            return std::size(r);
        }

        template <typename Rng>
            requires detail::concepts::sizeable_select_else<Rng>
        constexpr auto operator()(Rng &&gen) const -> std::size_t {
            auto count = static_cast<std::size_t>(0);
            for (auto &&_ : gen) { ++count; }
            return count;
        }
    };

    GENEX_EXPORT_STRUCT(size);


    struct empty_fn {
        template <typename Rng>
            requires detail::concepts::emptyable_select_empty<Rng>
        constexpr auto operator()(Rng &&r) const -> bool {
            return r.empty();
        }

        template <typename Rng>
            requires detail::concepts::emptyable_select_size<Rng>
        constexpr auto operator()(Rng &&rng) const -> bool {
            return operations::size(rng) == 0;
        }
    };

    GENEX_EXPORT_STRUCT(empty);
}


namespace genex {
    template <typename Rng>
    using range_size_t = decltype(operations::size(std::declval<Rng&>()));

    template <typename Rng>
    concept sized_range = operations::can_size_range<Rng> and requires { operations::size(std::declval<Rng&>()); };
}
