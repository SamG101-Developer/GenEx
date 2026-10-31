export module genex.detail.predicate_iters;
import genex.concepts;
import std;

export namespace genex::detail {
  /// What anything testing a projected element needs. An operation builds its
  /// own concept from this, as that's the name that shows up when a call
  /// doesn't compile.
  template <typename I, typename S, typename Pred, typename Proj>
  concept predicate_iters =
    std::input_iterator<I> and
    std::sentinel_for<S, I> and
    indirect_unary_predicate<Pred, projected<I, Proj>>;
}
