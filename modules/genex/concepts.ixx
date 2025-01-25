module;
#include <iterator>


export module genex.concepts;
import genex.iterators.begin;
import genex.iterators.end;
import genex.type_traits;

using namespace genex::type_traits;


namespace genex::concepts {
    export template <typename Rng>
    concept range = iterators::has_begin<Rng> and iterators::has_end<Rng>;

    // export template <typename E, typename Rng>
    // concept fixed_type_range = range<Rng> and std::same_as<range_value_t<Rng>, E>;

    export template <typename I>
    concept iterator = requires(I i)
    {
        { *i } -> std::convertible_to<typename I::value_type>;
        { ++i } -> std::same_as<I&>;
    };

    export template <typename S>
    concept sentinel = requires(S s)
    {
        { s == s } -> std::convertible_to<bool>;
        { s != s } -> std::convertible_to<bool>;
    };

    export template <typename T>
    concept callable = requires(T callable, auto &&... args)
    {
        std::invoke(callable, std::forward<decltype(args)>(args)...);
    };
}
