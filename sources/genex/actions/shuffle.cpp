module;
#include <genex/detail/macros.hpp>

export module genex.actions.shuffle;
export import genex.pipe;
import genex.algorithms.shuffle;
import genex.concepts;
import genex.meta;
import std;

namespace genex::actions::detail::concepts {
  template <typename Rng, typename Shuffler>
  concept shufflable_range =
    random_access_range<Rng> and
    permutable<iterator_t<Rng>> and
    std::uniform_random_bit_generator<std::remove_reference_t<Shuffler>>;
}

namespace genex::actions::detail {
  // Non-inline so the module's object file holds the one definition: MSVC
  // doesn't emit an inline thread_local variable defined in a module interface,
  // which fails at link time (LNK2001).
  auto default_random() -> std::mt19937_64& {
    thread_local std::mt19937_64 engine{std::random_device{}()};
    return engine;
  }
}

namespace genex::actions {
  struct shuffle_fn : pipe_closure {
    template <typename Rng>
    requires detail::concepts::shufflable_range<
      Rng, std::mt19937_64&>
    GENEX_INLINE auto operator()(
      Rng &&rng) const -> Rng {
      genex::shuffle(rng, detail::default_random());
      return std::forward<Rng>(rng);
    }

    template <typename Rng, typename Shuffler>
    requires detail::concepts::shufflable_range<Rng, Shuffler>
    GENEX_INLINE constexpr auto operator()(
      Rng &&rng, Shuffler shuffler) const -> Rng {
      genex::shuffle(rng, shuffler);
      return std::forward<Rng>(rng);
    }

    template <typename Shuffler>
    requires std::uniform_random_bit_generator<std::remove_cvref_t<Shuffler>>
    [[nodiscard]] GENEX_INLINE constexpr auto operator()(
      Shuffler shuffler) const noexcept(
      SAFE_CTOR(shuffle_fn) and SAFE_MOVE(Shuffler)) -> auto {
      return meta::bind_back(shuffle_fn{}, std::move(shuffler));
    }
  };

  export inline constexpr shuffle_fn shuffle{};
}
