export module genex.detail.algorithm_concepts;
import genex.concepts;
import genex.detail.predicate_iters;
import genex.operations.cmp;
import std;

namespace genex::algorithms::detail::concepts {
  export template <typename I, typename S, typename Pred, typename Proj>
  concept quantifiable_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;

  export template <typename Rng, typename Pred, typename Proj>
  concept quantifiable_range =
    input_range<Rng> and
    quantifiable_iters<iterator_t<Rng>, sentinel_t<Rng>, Pred, Proj>;

  export template <typename I, typename S, typename E, typename Proj>
  concept findable_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_equivalence_relation<
      operations::eq, projected<I, Proj>, std::remove_cvref_t<E> const*>;

  export template <typename I, typename S, typename Pred, typename Proj>
  concept findable_if_iters =
    genex::detail::predicate_iters<I, S, Pred, Proj>;
}
