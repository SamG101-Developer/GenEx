#pragma once
#include <utility>
#include <random>
#include <genex/concepts.hpp>
#include <genex/actions/_action_base.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>


namespace genex::actions::concepts {
    template <typename Rng, typename Shuffler>
    concept can_shuffle_range =
        random_access_range<Rng> and
        std::permutable<iterator_t<Rng>> and
        std::uniform_random_bit_generator<std::remove_reference_t<Shuffler>>;

    template <typename Rng>
    concept can_shuffle_random_range =
        random_access_range<Rng> and
        std::permutable<iterator_t<Rng>>;
}


namespace genex::actions {
    DEFINE_ACTION(shuffle) {
        template <typename Rng, typename Shuffler> requires concepts::can_shuffle_range<Rng, Shuffler>
        constexpr auto operator()(Rng &&rng, Shuffler &&shuffler) const -> Rng& {
            std::shuffle(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), std::forward<Shuffler>(shuffler));
            return rng;
        }

        template <typename Shuffler> requires (not input_range<std::remove_cvref_t<Shuffler>>)
        constexpr auto operator()(Shuffler &&shuffler) const -> auto {
            return
                [FWD_CAPTURES(shuffler)]<typename Rng> requires concepts::can_shuffle_range<Rng, Shuffler>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng), std::forward<Shuffler>(shuffler));
            };
        }
    };

    DEFINE_ACTION(shuffle_random) {
        template <typename Rng> requires concepts::can_shuffle_random_range<Rng>
        constexpr auto operator()(Rng &&rng) const -> Rng& {
            static thread_local std::mt19937_64 gen{std::random_device{}()};
            std::shuffle(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)), gen);
            return rng;
        }

        constexpr auto operator()() const -> auto {
            return
                [FWD_CAPTURES()]<typename Rng> requires concepts::can_shuffle_random_range<Rng>
                (Rng &&rng) mutable -> auto {
                return (*this)(std::forward<Rng>(rng));
            };
        }
    };

    EXPORT_GENEX_ACTION(shuffle);
    EXPORT_GENEX_ACTION(shuffle_random);
}
