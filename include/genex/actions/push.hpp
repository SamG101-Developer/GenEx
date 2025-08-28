#pragma once
#include <utility>
#include <genex/actions/_action_base.hpp>
#include <genex/concepts.hpp>
#include <genex/macros.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename E>
    concept can_push_back_range =
        range<Rng> and
        std::is_constructible_v<range_value_t<Rng>, E> and
        (has_member_push_back<Rng> or has_member_emplace_back<Rng> or has_member_insert<Rng>);

    template <typename Rng, typename E>
    concept can_push_front_range =
        range<Rng> and
        std::is_constructible_v<range_value_t<Rng>, E> and
        (has_member_push_front<Rng> or has_member_emplace_front<Rng> or has_member_insert<Rng>);

    template <typename Rng, typename I, typename E>
    concept can_push_range =
        range<Rng> and
        std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
        std::is_constructible_v<range_value_t<Rng>, E> and
        (has_member_push<Rng> or has_member_emplace<Rng> or has_member_insert<Rng>);
}


namespace genex::actions {
    DEFINE_ACTION(push_back) {
        template <typename Rng, typename E> requires (concepts::can_push_back_range<Rng, E> and has_member_emplace_back<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.emplace_back(std::forward<E>(elem)))) -> void {
            rng.emplace_back(std::forward<E>(elem));
        }

        template <typename Rng, typename E> requires (concepts::can_push_back_range<Rng, E> and has_member_push_back<Rng> and not has_member_emplace_back<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.push_back(std::forward<E>(elem)))) -> void {
            rng.push_back(std::forward<E>(elem));
        }

        template <typename Rng, typename E> requires (concepts::can_push_back_range<Rng, E> and has_member_insert<Rng> and not has_member_push_back<Rng> and not has_member_emplace_back<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.insert(end(rng), std::forward<E>(elem)))) -> void {
            rng.insert(end(rng), std::forward<E>(elem));
        }

        template <typename E> requires (not input_range<std::remove_cvref_t<E>>)
        auto operator()(E &&elem) const noexcept -> auto {
            return
                [FWD_CAPTURES(elem)]<typename Rng> requires concepts::can_push_back_range<Rng, E>
                (Rng &&rng) mutable -> void {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem));
            };
        }
    };

    DEFINE_ACTION(push_front) {
        template <typename Rng, typename E> requires (concepts::can_push_front_range<Rng, E> and has_member_emplace_front<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.emplace_front(std::forward<E>(elem)))) -> void {
            rng.emplace_front(std::forward<E>(elem));
        }

        template <typename Rng, typename E> requires (concepts::can_push_front_range<Rng, E> and has_member_push_front<Rng> and not has_member_emplace_front<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.push_front(std::forward<E>(elem)))) -> void {
            rng.push_front(std::forward<E>(elem));
        }

        template <typename Rng, typename E> requires (concepts::can_push_front_range<Rng, E> and has_member_insert<Rng> and not has_member_push_front<Rng> and not has_member_emplace_front<Rng>)
        auto operator()(Rng &&rng, E &&elem) const noexcept(noexcept(rng.insert(begin(rng), std::forward<E>(elem)))) -> void {
            rng.insert(begin(rng), std::forward<E>(elem));
        }

        template <typename E> requires (not input_range<std::remove_cvref_t<E>>)
        auto operator()(E &&elem) const noexcept -> auto {
            return
                [FWD_CAPTURES(elem)]<typename Rng> requires concepts::can_push_front_range<Rng, E>
                (Rng &&rng) mutable -> void {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem));
            };
        }
    };

    DEFINE_ACTION(push) {
        template <typename Rng, typename I, typename E> requires (concepts::can_push_range<Rng, I, E> and has_member_emplace<Rng>)
        auto operator()(Rng &&rng, I it, E &&elem) const noexcept(noexcept(rng.emplace(std::move(it), std::forward<E>(elem)))) -> void {
            rng.emplace(std::move(it), std::forward<E>(elem));
        }

        template <typename Rng, typename I, typename E> requires (concepts::can_push_range<Rng, I, E> and has_member_push<Rng> and not has_member_emplace<Rng>)
        auto operator()(Rng &&rng, I it, E &&elem) const noexcept(noexcept(rng.push(std::move(it), std::forward<E>(elem)))) -> void {
            rng.push(std::move(it), std::forward<E>(elem));
        }

        template <typename Rng, typename I, typename E> requires (concepts::can_push_range<Rng, I, E> and has_member_insert<Rng> and not has_member_push<Rng> and not has_member_emplace<Rng>)
        auto operator()(Rng &&rng, I it, E &&elem) const noexcept(noexcept(rng.insert(std::move(it), std::forward<E>(elem)))) -> void {
            rng.insert(std::move(it), std::forward<E>(elem));
        }

        template <typename I, typename E> requires (not input_range<std::remove_cvref_t<I>>)
        auto operator()(I it, E &&elem) const noexcept -> auto {
            return
                [FWD_CAPTURES(it, elem)]<typename Rng> requires concepts::can_push_range<Rng, I, E>
                (Rng &&rng) mutable -> void {
                return (*this)(std::forward<Rng>(rng), std::forward<I>(it), std::forward<E>(elem));
            };
        }
    };

    EXPORT_GENEX_ACTION(push_back);
    EXPORT_GENEX_ACTION(push_front);
    EXPORT_GENEX_ACTION(push);
}
