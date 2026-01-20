module;
#include <genex/macros.hpp>

export module genex.conditional.if_;
export import genex.pipe;
import genex.concepts;
import genex.meta;
import std;


namespace genex::conditional::detail::concepts {
    template <typename Rng, typename Pred, typename Then>
    concept conditionally_range =
        std::ranges::input_range<Rng> and
        std::invocable<Pred, Rng> and
        std::invocable<Then> and
        std::same_as<std::invoke_result_t<Pred, Rng>, bool>;
}


namespace genex::conditional::detail::impl {
    template <typename Rng, typename Pred, typename Then>
    requires concepts::conditionally_range<Rng, Pred, Then>
    GENEX_INLINE constexpr auto do_if(Rng &&rng, Pred &&pred, Then &&then) -> decltype(auto) {
        if (meta::invoke(std::forward<Pred>(pred), std::forward<Rng>(rng))) {
            meta::invoke(std::forward<Then>(then));
        }
        return std::forward<Rng>(rng);
    }

    template <typename Rng, typename Pred, typename Then>
    requires concepts::conditionally_range<Rng, Pred, Then>
    GENEX_INLINE constexpr auto do_if_not(Rng &&rng, Pred &&pred, Then &&then) -> decltype(auto) {
        if (not meta::invoke(std::forward<Pred>(pred), std::forward<Rng>(rng))) {
            meta::invoke(std::forward<Then>(then));
        }
        return std::forward<Rng>(rng);
    }
}


namespace genex {
    struct if_fn {
        template <typename Rng, typename Pred, typename Then>
        requires conditional::detail::concepts::conditionally_range<Rng, Pred, Then>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Then &&then) const -> decltype(auto) {
            return conditional::detail::impl::do_if(std::forward<Rng>(rng), std::move(pred), std::forward<Then>(then));
        }

        template <typename Pred, typename Then>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred &&pred, Then &&then) const -> auto {
            return meta::bind_back(if_fn{}, std::forward<Pred>(pred), std::forward<Then>(then));
        }
    };

    struct if_not_fn {
        template <typename Rng, typename Pred, typename Then>
        requires conditional::detail::concepts::conditionally_range<Rng, Pred, Then>
        GENEX_INLINE constexpr auto operator()(Rng &&rng, Pred &&pred, Then &&then) const -> decltype(auto) {
            return conditional::detail::impl::do_if_not(std::forward<Rng>(rng), std::move(pred), std::forward<Then>(then));
        }

        template <typename Pred, typename Then>
        requires (not range<Pred>)
        GENEX_INLINE constexpr auto operator()(Pred &&pred, Then &&then) const -> auto {
            return meta::bind_back(if_not_fn{}, std::forward<Pred>(pred), std::forward<Then>(then));
        }
    };

    export inline constexpr if_fn if_{};
    export inline constexpr if_not_fn if_not{};
}
