#pragma once
#include <functional>
#include <utility>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename I, typename E>
    concept insertable_range =
        input_range<Rng> and
        std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
        std::constructible_from<range_value_t<Rng>, E&&>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_emplace =
        insertable_range<Rng, I, E> and
        has_member_emplace<Rng>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_push =
        insertable_range<Rng, I, E> and
        has_member_push<Rng> and
        not has_member_emplace<Rng>;

    template <typename Rng, typename I, typename E>
    concept insertable_select_insert =
        insertable_range<Rng, I, E> and
        has_member_insert<Rng> and
        not has_member_push<Rng> and
        not has_member_emplace<Rng>;
}


namespace genex::actions {
    struct insert_fn {
        template <typename Rng, typename I, typename E>
            requires concepts::insertable_select_emplace<Rng, I, E>
        constexpr auto operator()(Rng &&rng, I it, E &&elem) const -> auto {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            return rng.emplace(std::move(it), std::forward<E>(elem));
        }

        template <typename Rng, typename I, typename E>
            requires concepts::insertable_select_push<Rng, I, E>
        constexpr auto operator()(Rng &&rng, I it, E &&elem) const -> auto {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            return rng.push(std::move(it), std::forward<E>(elem));
        }

        template <typename Rng, typename I, typename E>
            requires concepts::insertable_select_insert<Rng, I, E>
        constexpr auto operator()(Rng &&rng, I it, E &&elem) const -> auto {
            GENEX_ASSERT(std::out_of_range, it >= std::begin(rng) and it <= std::end(rng));
            return rng.insert(std::move(it), std::forward<E>(elem));
        }

        template <typename I, typename E>
            requires (not input_range<std::remove_cvref_t<I>>)
        constexpr auto operator()(I it, E &&elem) const -> auto {
            return std::bind_back(insert_fn{}, std::move(it), std::forward<E>(elem));
        }
    };

    GENEX_EXPORT_STRUCT(insert);
}
