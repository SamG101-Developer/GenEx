module;
#include <functional>

export module genex.algorithms.find;
import genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.meta;
import genex.type_traits;

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::algorithms {
    struct find_base_fn {
        template <iterator I, sentinel S, typename E, std::invocable<E> Proj = meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
        auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> I {
            for (auto it = first; it != last; ++it) {
                if (std::invoke(std::forward<Proj>(proj), *it) == elem) { return it; }
            }
            return last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        auto operator()(Rng &&rng, range_value_t<Rng> const &elem, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), elem, std::forward<Proj>(proj));
        }
    };

    struct find_last_base_fn {
        template <typename I, typename S, typename E, std::invocable<E> Proj = meta::identity> requires std::equality_comparable_with<iter_value_t<I>, E>
        auto operator()(I &&first, S &&last, E &&elem, Proj &&proj = {}) const -> I {
            auto found_last = last;
            for (auto it = first; it != last; ++it) {
                if (std::invoke(std::forward<Proj>(proj), *it) == elem) { found_last = it; }
            }
            return found_last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity>
        auto operator()(Rng &&rng, range_value_t<Rng> const &elem, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), elem, std::forward<Proj>(proj));
        }
    };

    struct find_if_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            for (auto it = first; it != last; ++it) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) { return it; }
            }
            return last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    struct find_last_if_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            auto found_last = last;
            for (auto it = first; it != last; ++it) {
                if (std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) { found_last = it; }
            }
            return found_last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };


    struct find_if_not_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            for (auto it = first; it != last; ++it) {
                if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) { return it; }
            }
            return last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    struct find_last_if_not_base_fn {
        template <iterator I, sentinel S, std::invocable<iter_value_t<I>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, iter_value_t<I>>> Pred>
        auto operator()(I &&first, S &&last, Pred &&pred, Proj &&proj = {}) const -> I {
            auto found_last = last;
            for (auto it = first; it != last; ++it) {
                if (!std::invoke(std::forward<Pred>(pred), std::invoke(std::forward<Proj>(proj), *it))) { found_last = it; }
            }
            return found_last;
        }

        template <range Rng, std::invocable<range_value_t<Rng>> Proj = meta::identity, std::predicate<std::invoke_result_t<Proj, range_value_t<Rng>>> Pred>
        auto operator()(Rng &&rng, Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return (*this)(iterators::begin(rng), iterators::end(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
        }
    };

    struct find_fn final : find_base_fn {
        using find_base_fn::operator();

        template <typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            return [elem = std::forward<E>(elem), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<Proj>(proj));
            };
        }
    };

    struct find_last_fn final : find_last_base_fn {
        using find_last_base_fn::operator();

        template <typename E, std::invocable<E> Proj = meta::identity>
        auto operator()(E &&elem, Proj &&proj = {}) const -> decltype(auto) {
            return [elem = std::forward<E>(elem), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<E>(elem), std::forward<Proj>(proj));
            };
        }
    };

    struct find_if_fn final : find_if_base_fn {
        using find_if_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    struct find_last_if_fn final : find_last_if_base_fn {
        using find_last_if_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    struct find_if_not_fn final : find_if_not_base_fn {
        using find_if_not_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    struct find_last_if_not_fn final : find_last_if_not_base_fn {
        using find_last_if_not_base_fn::operator();

        template <typename Pred, typename Proj = meta::identity>
        auto operator()(Pred &&pred, Proj &&proj = {}) const -> decltype(auto) {
            return [pred = std::forward<Pred>(pred), proj = std::forward<Proj>(proj), this]<range Rng>(Rng &&rng) mutable {
                return (*this)(std::forward<Rng>(rng), std::forward<Pred>(pred), std::forward<Proj>(proj));
            };
        }
    };

    export inline constexpr find_fn find;
    export inline constexpr find_last_fn find_last;
    export inline constexpr find_if_fn find_if;
    export inline constexpr find_last_if_fn find_last_if;
    export inline constexpr find_if_not_fn find_if_not;
    export inline constexpr find_last_if_not_fn find_last_if_not;
}
