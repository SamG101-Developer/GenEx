export module genex.closure;
import std;

export namespace genex {
  /// The base of everything that can go on the right of a pipe. It is also what
  /// puts "genex" among the namespaces searched for the pipe operators, which
  /// is how they're found from anywhere without being global.
  struct pipe_closure {};

  template <typename F>
  concept pipeable = std::derived_from<std::remove_cvref_t<F>, pipe_closure>;
}
