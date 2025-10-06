#pragma once
#include <tuple>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>
#include <genex/operations/data.hpp>
#include <genex/operations/size.hpp>


namespace genex::views::detail::concepts {
    template <typename T1, typename T2>
    struct concatenatable_iters_helper : std::false_type {
    };

    template <typename... Is, typename... Ss>
    struct concatenatable_iters_helper<std::tuple<Is...>, std::tuple<Ss...>> {
        static constexpr bool value =
            sizeof...(Is) > 0 and
            sizeof...(Is) == sizeof...(Ss) and
            (std::input_iterator<Is> and ...) and
            (std::sentinel_for<Ss, Is> and ...) and
            (std::indirectly_readable<Is> and ...) and
            (std::movable<Is> and ...);
    };

    template <typename T1, typename T2>
    concept concatenatable_iters =
        concatenatable_iters_helper<T1, T2>::value;

    template <typename... Rngs>
    concept concatenatable_range =
        (input_range<Rngs> and ...) and
        concatenatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>;
}


namespace genex::views::detail {
    template <typename ...Rngs>
    requires concepts::concatenatable_range<Rngs...>
    struct concat_iterator {
        static constexpr auto N = sizeof...(Rngs);

        using reference = std::common_reference_t<range_reference_t<Rngs>...>;
        using value_type = std::common_type_t<range_value_t<Rngs>...>;
        using pointer = std::add_pointer_t<value_type>;

        using iterator_concept =
            std::conditional_t<(std::random_access_iterator<iterator_t<Rngs>> and ...), std::random_access_iterator_tag,
            std::conditional_t<(std::bidirectional_iterator<iterator_t<Rngs>> and ...), std::bidirectional_iterator_tag,
            std::conditional_t<(std::forward_iterator<iterator_t<Rngs>> and ...), std::forward_iterator_tag, std::input_iterator_tag>>>;
        using iterator_category = iterator_concept;
        using difference_type = std::ptrdiff_t;

        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;
        std::size_t index = N;

        GENEX_INLINE constexpr explicit concat_iterator(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts)  :
            its(std::move(its)), sts(std::move(sts)), index(0) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept(
            noexcept(get_deref_impl(0))) -> reference {
            return get_deref_impl(index);
        }

        GENEX_INLINE constexpr auto operator*() noexcept(
            noexcept(get_deref_impl(0))) -> reference {
            return get_deref_impl(index);
        }

        GENEX_INLINE constexpr auto operator++() noexcept(
            noexcept(advance_impl(0))) -> concat_iterator& {
            if (index < N) { advance_impl(index); }
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int) noexcept(
            noexcept(advance_impl(0))) -> concat_iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

    private:
        template <std::size_t I = 0>
        constexpr reference get_deref_impl(const std::size_t idx) const noexcept {
            if constexpr (I + 1 == N) {
                return *std::get<I>(its);
            } else {
                if (idx == I) return *std::get<I>(its);
                return get_deref_impl<I + 1>(idx);
            }
        }

        template <std::size_t I = 0>
        constexpr reference get_deref_impl(const std::size_t idx) noexcept {
            if constexpr (I + 1 == N) {
                return *std::get<I>(its);
            } else {
                if (idx == I) return *std::get<I>(its);
                return get_deref_impl<I + 1>(idx);
            }
        }

        template <std::size_t I = 0>
        constexpr void advance_impl(const std::size_t idx) {
            if constexpr (I + 1 == N) {
                if (idx == I) {
                    auto &it = std::get<I>(its);
                    auto &st = std::get<I>(sts);
                    ++it;
                    if (it == st) index = N;
                }
            } else {
                if (idx == I) {
                    auto &it = std::get<I>(its);
                    auto &st = std::get<I>(sts);
                    ++it;
                    if (it == st) {
                        ++index;
                        normalize();
                    }
                } else {
                    advance_impl<I + 1>(idx);
                }
            }
        }

        template <std::size_t I = 0>
        constexpr void normalize_impl(const std::size_t idx) {
            if constexpr (I + 1 == N) {
                if (idx == I) {
                    if (std::get<I>(its) == std::get<I>(sts)) index = N;
                }
            } else {
                if (idx == I) {
                    if (std::get<I>(its) == std::get<I>(sts)) {
                        ++index;
                        normalize_impl<I + 1>(index);
                    }
                } else {
                    normalize_impl<I + 1>(idx);
                }
            }
        }

        constexpr void normalize() {
            if (index >= N) return;
            for (;;) {
                if (index >= N) { index = N; return; }
                if (not current_range_empty(index)) return;
                ++index;
            }
        }

        template <std::size_t I = 0>
        GENEX_NODISCARD constexpr bool current_range_empty_impl(const std::size_t idx) const {
            if constexpr (I + 1 == N) {
                if (idx == I) return std::get<I>(its) == std::get<I>(sts);
                return true;
            } else {
                if (idx == I) return std::get<I>(its) == std::get<I>(sts);
                return current_range_empty_impl<I + 1>(idx);
            }
        }

        GENEX_NODISCARD constexpr bool current_range_empty(const std::size_t idx) const {
            return current_range_empty_impl( idx );
        }
    };

    struct concat_sentinel { };

    template <typename ...Rngs>
    requires concepts::concatenatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>
    concat_iterator(std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>) -> concat_iterator<Rngs...>;

    template <typename ...Rngs>
    requires concepts::concatenatable_iters<std::tuple<iterator_t<Rngs>...>, std::tuple<sentinel_t<Rngs>...>>
    GENEX_VIEW_ITERSENT_EQOP_DEFINITIONS(concat_iterator, concat_sentinel, Rngs...) {
        return std::get<sizeof...(Rngs) - 1>(it.its) == std::get<sizeof...(Rngs) - 1>(it.sts);
    }

    template <typename ...Vs>
    requires concepts::concatenatable_range<Vs...> and (sizeof...(Vs) > 1)
    struct concat_view : std::ranges::view_interface<concat_view<Vs...>> {
        std::tuple<Vs...> base_rngs;

        GENEX_INLINE constexpr concat_view() noexcept = default;

        GENEX_VIEW_VIEW_FUNC_DEFINITIONS(
            concat_iterator, concat_sentinel, base_rngs)

        GENEX_INLINE constexpr explicit concat_view(Vs ...rngs) noexcept(
            (std::is_nothrow_move_constructible_v<Vs> and ...)) :
            base_rngs(std::move(rngs)...) {
        }

        GENEX_INLINE constexpr auto internal_begin() const noexcept(
            noexcept(std::apply([](auto &...rngs) { return std::make_tuple(iterators::begin(rngs)...); }, base_rngs))) {
            return std::make_from_tuple<std::tuple<iterator_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::begin(rngs)...); }, base_rngs));
        }

        GENEX_INLINE constexpr auto internal_end() const noexcept(
            noexcept(std::apply([](auto &...rngs) { return std::make_tuple(iterators::end(rngs)...); }, base_rngs))) {
            return std::make_from_tuple<std::tuple<sentinel_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::end(rngs)...); }, base_rngs));
        }
    };
}


namespace genex::views {
    struct concat_fn {
        template <typename ...Rng>
        requires (sizeof...(Rng) > 1 and (detail::concepts::concatenatable_range<Rng> and ...))
        GENEX_INLINE constexpr auto operator()(Rng &&...rng) const noexcept -> auto {
            return detail::concat_view<std::views::all_t<Rng>...>{
                std::views::all(std::forward<Rng>(rng))...};
        }

        template <typename Rng>
        requires detail::concepts::concatenatable_range<Rng>
        GENEX_INLINE constexpr auto operator()(Rng &&rng) const noexcept -> auto {
            return std::bind_back(
                concat_fn{}, std::views::all(std::forward<Rng>(rng)));
        }
    };

    inline constexpr concat_fn concat{};
}
