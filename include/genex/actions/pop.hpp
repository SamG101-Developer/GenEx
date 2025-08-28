#pragma once
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/distance.hpp>
#include <genex/iterators/next.hpp>
#include <genex/operations/size.hpp>


namespace genex::actions::concepts {
    template <typename Rng>
    concept can_pop_back_range =
        range<Rng> and
        (has_member_pop_back<Rng> or has_member_pop<Rng> or has_member_erase<Rng>);

    template <typename Rng>
    concept can_pop_front_range =
        range<Rng> and
        (has_member_pop_front<Rng> or has_member_pop<Rng> or has_member_erase<Rng>);

    template <typename Rng, typename I>
    concept can_pop_range =
        range<Rng> and
        std::same_as<std::remove_cvref_t<I>, iterator_t<Rng>> and
        (has_member_pop<Rng> or has_member_erase<Rng>);
}


namespace genex::actions {
    DEFINE_ACTION(pop_back) {
        template <typename Rng> requires (concepts::can_pop_back_range<Rng> and has_member_pop_back<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.pop_back())) -> auto {
            return rng.pop_back();
        }

        template <typename Rng> requires (concepts::can_pop_back_range<Rng> and not has_member_pop_back<Rng> and has_member_pop<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.pop(operations::size(rng) - 1))) -> auto {
            return rng.pop(operations::size(rng) - 1);
        }

        template <typename Rng> requires (concepts::can_pop_back_range<Rng> and not has_member_pop_back<Rng> and not has_member_pop<Rng> and has_member_erase<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.erase(iterators::prev(iterators::end(rng))))) -> auto {
            return rng.erase(iterators::prev(iterators::end(rng)));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_pop_back_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    DEFINE_ACTION(pop_front) {
        template <typename Rng> requires (concepts::can_pop_front_range<Rng> and has_member_pop_front<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.pop_front())) -> auto {
            return rng.pop_front();
        }

        template <typename Rng> requires (concepts::can_pop_front_range<Rng> and has_member_pop<Rng> and not has_member_pop_front<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.pop(0))) -> auto {
            return rng.pop(0);
        }

        template <typename Rng> requires (concepts::can_pop_front_range<Rng> and has_member_erase<Rng> and not has_member_pop_front<Rng> and not has_member_pop<Rng>)
        auto operator()(Rng &&rng) const noexcept(noexcept(rng.erase(iterators::begin(rng)))) -> auto {
            return rng.erase(iterators::begin(rng));
        }

        auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_pop_front_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };


    DEFINE_ACTION(pop) {
        template <typename Rng, typename I> requires (concepts::can_pop_range<Rng, I> and has_member_pop<Rng>)
        auto operator()(Rng &&rng, I it) const noexcept(noexcept(rng.pop(iterators::distance(iterators::begin(rng), std::move(it))))) -> auto {
            return rng.pop(iterators::distance(iterators::begin(rng), std::move(it)));
        }

        template <typename Rng, typename I> requires (concepts::can_pop_range<Rng, I> and has_member_erase<Rng> and not has_member_pop<Rng>)
        auto operator()(Rng &&rng, I it) const noexcept(noexcept(rng.erase(std::move(it)))) -> auto {
            return rng.erase(std::move(it));
        }

        template <typename I> requires (not input_range<std::remove_cvref_t<I>>)
        auto operator()(I it) const -> auto {
            return
                [FWD_CAPTURES(it)]<typename Rng> requires concepts::can_pop_range<Rng, I>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), it);
            };
        }
    };

    EXPORT_GENEX_ACTION(pop_back);
    EXPORT_GENEX_ACTION(pop_front);
    EXPORT_GENEX_ACTION(pop);
}
