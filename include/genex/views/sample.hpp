#pragma once
#include <genex/concepts.hpp>
#include <genex/generator.hpp>
#include <genex/macros.hpp>
#include <genex/pipe.hpp>
#include <genex/iterators/iter_pair.hpp>


namespace genex::views::detail::concepts {
    template <typename I, typename S, typename Int, typename Shuffler>
    concept samplable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::weakly_incrementable<Int> and
        std::uniform_random_bit_generator<std::remove_reference_t<Shuffler>>;

    template <typename Rng, typename Int, typename Shuffler>
    concept samplable_ranges =
        input_range<Rng> and
        samplable_iters<iterator_t<Rng>, sentinel_t<Rng>, Int, Shuffler>;
}


// namespace genex::views::detail::impl {
//     template <typename I, typename S, typename Int, typename Shuffler>
//     requires concepts::samplable_iters<I, S, Int, Shuffler>
//     auto do_sample(I first, S last, Int n, Shuffler shuffler) -> generator<iter_value_t<I>>
// }
