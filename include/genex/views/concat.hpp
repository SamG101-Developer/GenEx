#pragma once
#include <tuple>

#include <genex/concepts.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/access.hpp>


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
            (std::movable<Is> and ...) and
            requires { typename std::common_type_t<iter_value_t<Is>...>; };
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

        GENEX_INLINE constexpr explicit concat_iterator() = default;

        std::tuple<iterator_t<Rngs>...> its;
        std::tuple<sentinel_t<Rngs>...> sts;
        std::size_t index = N;

        GENEX_INLINE constexpr explicit concat_iterator(std::tuple<iterator_t<Rngs>...> its, std::tuple<sentinel_t<Rngs>...> sts) noexcept(
            (std::is_nothrow_move_constructible_v<iterator_t<Rngs>> and ...) &&
            (std::is_nothrow_move_constructible_v<sentinel_t<Rngs>> and ...)) :
            its(std::move(its)), sts(std::move(sts)), index(0) {
        }

        GENEX_INLINE constexpr auto operator*() const noexcept
            -> reference {
            return get_deref_impl(index);
        }

        GENEX_INLINE constexpr auto operator->() const noexcept
            -> pointer {
            return std::addressof(get_deref_impl(index));
        }

        GENEX_INLINE constexpr auto operator++() noexcept
            -> concat_iterator& {
            if (index < N) advance_impl(index);
            return *this;
        }

        GENEX_INLINE constexpr auto operator++(int)
            -> concat_iterator {
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
        constexpr void advance_impl(const std::size_t idx) {
            if constexpr (I + 1 == N) {
                if (idx == I) {
                    auto &it = std::get<I>(its);
                    auto &st = std::get<I>(sts);
                    ++it;
                    if (it == st) index = N; // mark end
                }
            } else {
                if (idx == I) {
                    auto &it = std::get<I>(its);
                    auto &st = std::get<I>(sts);
                    ++it;
                    if (it == st) {
                        ++index;
                        normalize(); // skip possible empty ranges after increment
                    }
                } else {
                    advance_impl<I + 1>(idx);
                }
            }
        }

        template <std::size_t I = 0>
        constexpr void normalize_impl(std::size_t idx) {
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
            // find first non-empty range starting from index (initially 0)
            if (index >= N) return;
            // fast loop: repeated normalize_impl will move index forward until a non-empty is found or index==N
            for (;;) {
                if (index >= N) { index = N; return; }
                auto empty = false;
                // check current range empty by using a small manual unrolled sequence via recursion
                empty = current_range_empty(index);
                if (!empty) return;
                ++index;
            }
        }

        template <std::size_t I = 0>
        constexpr bool current_range_empty_impl(std::size_t idx) const {
            if constexpr (I + 1 == N) {
                if (idx == I) return std::get<I>(its) == std::get<I>(sts);
                return true; // shouldn't happen
            } else {
                if (idx == I) return std::get<I>(its) == std::get<I>(sts);
                return current_range_empty_impl<I + 1>(idx);
            }
        }

        constexpr bool current_range_empty(std::size_t idx) const {
            return current_range_empty_impl( idx );
        }
    };

    template <typename ...Rngs>
    struct concat_sentinel {
        static constexpr auto N = sizeof...(Rngs);

        GENEX_INLINE constexpr concat_sentinel() = default;

        GENEX_INLINE constexpr auto operator==(concat_iterator<Rngs...> const &it) const -> bool {
            return it.index == N;
        }
    };

    template <typename ...Vs>
    requires concepts::concatenatable_range<Vs...> and (sizeof...(Vs) > 1)
    struct concat_view : std::ranges::view_interface<concat_view<Vs...>> {
        GENEX_INLINE constexpr explicit concat_view() = default;;

        using iterator = concat_iterator<Vs...>;
        using sentinel = concat_sentinel<Vs...>;

        std::tuple<Vs...> base_rngs;

        GENEX_INLINE constexpr explicit concat_view(Vs ...rngs) :
            base_rngs(std::move(rngs)...) {
        }

        GENEX_INLINE constexpr auto begin() noexcept(
            (std::is_nothrow_copy_constructible_v<iterator_t<Vs>> and ...))
            -> iterator {
            return iterator{
                std::make_from_tuple<std::tuple<iterator_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::begin(rngs)...); }, base_rngs)),
                std::make_from_tuple<std::tuple<sentinel_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::end(rngs)...); }, base_rngs))};
        }

        GENEX_INLINE constexpr auto begin() const noexcept(
            (std::is_nothrow_copy_constructible_v<iterator_t<Vs>> and ...))
            -> iterator requires (range<const Vs> and ...) {
            return iterator{
                std::make_from_tuple<std::tuple<iterator_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::begin(rngs)...); }, base_rngs)),
                std::make_from_tuple<std::tuple<sentinel_t<Vs>...>>(std::apply([](auto &...rngs) { return std::make_tuple(iterators::end(rngs)...); }, base_rngs))};
        }

        GENEX_INLINE constexpr auto end() noexcept(
            (std::is_nothrow_copy_constructible_v<sentinel_t<Vs>> and ...))
            -> sentinel {
            return sentinel{};
        }

        GENEX_INLINE constexpr auto end() const noexcept(
            (std::is_nothrow_copy_constructible_v<sentinel_t<Vs>> and ...))
            -> sentinel requires (range<Vs*> and ...) {
            return sentinel{};
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
                concat_fn{}, std::forward<Rng>(rng));
        }
    };

    inline constexpr concat_fn concat{};
}
