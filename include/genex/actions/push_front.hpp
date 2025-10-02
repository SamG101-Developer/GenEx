#pragma once
#include <genex/pipe.hpp>
#include <genex/actions/insert.hpp>
#include <genex/iterators/access.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename E>
    concept front_insertable_range =
        range<Rng> and
        std::constructible_from<range_value_t<Rng>, E&&>;

    template <typename Rng, typename E>
    concept front_insertable_select_emplace_front =
        front_insertable_range<Rng, E> and
        has_member_emplace_front<Rng>;

    template <typename Rng, typename E>
    concept front_insertable_select_push_front =
        front_insertable_range<Rng, E> and
        has_member_push_front<Rng> and
        not has_member_emplace_front<Rng>;

    template <typename Rng, typename E>
    concept front_insertable_select_insert =
        front_insertable_range<Rng, E> and
        has_member_insert<Rng> and
        not has_member_push_front<Rng> and
        not has_member_emplace_front<Rng>;
}


namespace genex::actions {
    struct push_front_fn {
        template <typename Rng, typename E>
            requires concepts::front_insertable_select_emplace_front<Rng, E>
        constexpr auto operator()(Rng &&rng, E &&elem) const -> auto {
            return rng.emplace_front(std::forward<E>(elem));
        }

        template <typename Rng, typename E>
            requires concepts::front_insertable_select_push_front<Rng, E>
        constexpr auto operator()(Rng &&rng, E &&elem) const -> auto {
            return rng.push_front(std::forward<E>(elem));
        }

        template <typename Rng, typename E>
            requires concepts::front_insertable_select_insert<Rng, E>
        constexpr auto operator()(Rng &&rng, E &&elem) const -> auto {
            return actions::insert(rng, iterators::begin(rng), std::forward<E>(elem));
        }

        template <typename E>
        constexpr auto operator()(E &&elem) const -> auto {
            return std::bind_back(push_front_fn{}, std::forward<E>(elem));
        }
    };

    GENEX_EXPORT_STRUCT(push_front);
}
