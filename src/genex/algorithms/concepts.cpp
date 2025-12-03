export module genex.algorithms.concepts;
import genex.concepts;
import std;


namespace genex::algorithms::detail::concepts {
    export template <typename I, typename S, typename Pred, typename Proj>
    concept quantifiable_iters =
        std::input_iterator<I> and
        std::sentinel_for<S, I> and
        std::indirect_unary_predicate<Pred, std::projected<I, Proj>>;

    export template <typename Rng, typename Pred, typename Proj>
    concept quantifiable_range =
        input_range<Rng> and
        quantifiable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;
}
