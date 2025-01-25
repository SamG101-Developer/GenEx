module;
#include <functional>

export module genex.views.to;
import genex.type_traits;
import genex.iterators.begin;
import genex.iterators.end;

using namespace genex::type_traits;


namespace genex::views {
    template <template <typename> typename Out, typename Rng>
    auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>>;

    template <template <typename> typename Out>
    auto to_fn() -> decltype(auto);

    export template <template <typename> typename Out>
    auto to() -> decltype(auto);
}


template <template <typename> class Out, typename Rng = void>
auto genex::views::to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    return Out<range_value_t<Rng>>{iterators::begin(rng), iterators::end(rng)};
}

template <template <typename> class Out>
auto genex::views::to_fn() -> decltype(auto) {
    return []<typename Rng>(Rng &&rng) { return to_base_fn<Out>(std::forward<Rng>(rng)); };
}

template <template <typename> class Out>
auto genex::views::to() -> decltype(auto) {
    return to_fn<Out>();
}
