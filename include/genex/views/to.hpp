#pragma once
#include <coroutine>
#include <genex/categories.hpp>
#include <genex/iterators/begin.hpp>
#include <genex/iterators/end.hpp>
#include <genex/concepts.hpp>
#include <genex/views/map.hpp>

using namespace genex::concepts;
using namespace genex::type_traits;


namespace genex::views {
    template <typename Rng>
    struct InnerRangeType;

    template <typename Rng>
    using InnerRangeTypeT = typename InnerRangeType<std::remove_cvref_t<Rng>>::type;


    // template <template <typename> typename Out, categories::non_generator_range Rng>
    // auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
    //     return Out<range_value_t<Rng>>(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
    // }

    template <template <typename> typename Out, range Rng> // requires (not range<range_value_t<Rng>>)
    auto to_base_fn(Rng &&rng) -> Out<range_value_t<Rng>> {
        Out<range_value_t<Rng>> out;
        auto begin = iterators::begin(rng);
        auto end = iterators::end(rng);
        for (; begin != end; ++begin) {
            out.push_back(*begin);
        }
        return out;
    }

    // template <template <typename...> typename Out, range Rng> requires (range<range_value_t<Rng>>)
    // auto to_base_fn(Rng &&rng) -> Out<InnerRangeTypeT<Rng>> {
    //     auto inner = rng | views::map([]<typename Inner>(Inner &&x) { return to_base_fn<Out>(std::forward<Inner>(x)); });
    //     return to_base_fn<Out>(std::move(inner));
    // }

    // template <typename Out, categories::non_generator_range Rng>
    // auto to_base_fn(Rng &&rng) -> Out {
    //     return Out(iterators::begin(std::forward<Rng>(rng)), iterators::end(std::forward<Rng>(rng)));
    // }

    template <typename Out, range Rng>
    auto to_base_fn(Rng &&rng) -> Out {
        Out out;
        auto begin = iterators::begin(rng);
        auto end = iterators::end(rng);
        for (; begin != end; ++begin) {
            out.push_back(*begin);
        }
        return out;
    }

    template <template <typename...> typename Out>
    auto to() -> decltype(auto) {
        return []<range Rng>(Rng &&rng) { return to_base_fn<Out>(std::forward<Rng>(rng)); };
    }

    template <typename Out>
    auto to() -> decltype(auto) {
        return []<range Rng>(Rng &&rng) { return to_base_fn<Out>(std::forward<Rng>(rng)); };
    }
}


template <typename Rng>
struct genex::views::InnerRangeType {
    using type = std::remove_cvref_t<Rng>;
};


template <typename Rng> requires genex::is_generator<range_value_t<Rng>>
struct genex::views::InnerRangeType<Rng> {
    using type = typename InnerRangeType<typename generator<range_value_t<Rng>>::iterator::value_type>::type;
};
