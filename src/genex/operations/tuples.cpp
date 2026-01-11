export module genex.operations.tuples;
import std;
import genex.concepts;


export namespace genex::operations::tuples {
    template <typename ...Rngs, std::size_t... Is>
    constexpr auto deref_tuple_impl(std::tuple<iterator_t<Rngs>...> &its, std::index_sequence<Is...>) -> decltype(auto) {
        return std::tie((*std::get<Is>(its))...);
    }

    template <typename ...Rngs>
    constexpr auto deref_tuple(std::tuple<iterator_t<Rngs>...> &its) -> decltype(auto) {
        return deref_tuple_impl(its, std::index_sequence_for<Rngs...>{});
    }

    template <typename ...Rngs, std::size_t... Is>
    constexpr auto advance_tuple_impl(std::tuple<iterator_t<Rngs>...> &its, std::index_sequence<Is...>) -> void {
        (..., ++std::get<Is>(its));
    }

    template <typename ...Rngs>
    constexpr auto advance_tuple(std::tuple<iterator_t<Rngs>...> &its) -> void {
        advance_tuple_impl(its, std::index_sequence_for<Rngs...>{});
    }

    template <typename ...Rngs, std::size_t... Is>
    constexpr auto advance_tuple_impl(std::tuple<iterator_t<Rngs>...> &its, std::common_type_t<range_difference_t<Rngs>...> n, std::index_sequence<Is...>) -> void {
        (..., (std::get<Is>(its) += n));
    }

    template <typename ...Rngs>
    constexpr auto advance_tuple(std::tuple<iterator_t<Rngs>...> &its, std::common_type_t<range_difference_t<Rngs>...> n) -> void {
        advance_tuple_impl(its, n, std::index_sequence_for<Rngs...>{});
    }

    template <typename ...Rngs, std::size_t... Is>
    constexpr auto retreat_tuple_impl(std::tuple<iterator_t<Rngs>...> &its, std::index_sequence<Is...>) -> void {
        (..., --std::get<Is>(its));
    }

    template <typename ...Rngs>
    constexpr auto retreat_tuple(std::tuple<iterator_t<Rngs>...> &its) -> void {
        retreat_tuple_impl(its, std::index_sequence_for<Rngs...>{});
    }

    template <typename ...Rngs, std::size_t... Is>
    constexpr auto retreat_tuple_impl(std::tuple<iterator_t<Rngs>...> &its, std::common_type_t<range_difference_t<Rngs>...> n, std::index_sequence<Is...>) -> void {
        (..., (std::get<Is>(its) -= n));
    }

    template <typename ...Rngs>
    constexpr auto retreat_tuple(std::tuple<iterator_t<Rngs>...> &its, std::common_type_t<range_difference_t<Rngs>...> n) -> void {
        retreat_tuple_impl(its, n, std::index_sequence_for<Rngs...>{});
    }

    template <typename ...Rngs, std::size_t... Is>
    constexpr auto distance_tuple_impl(
        std::tuple<iterator_t<Rngs>...> const &a, std::tuple<iterator_t<Rngs>...> const &b, std::index_sequence<Is...>) -> std::common_type_t<range_difference_t<Rngs>...> {
        return (... + (std::get<Is>(a) - std::get<Is>(b)));
    }

    template <typename ...Rngs>
    constexpr auto distance_tuple(
        std::tuple<iterator_t<Rngs>...> const &a, std::tuple<iterator_t<Rngs>...> const &b) -> std::common_type_t<range_difference_t<Rngs>...> {
        return distance_tuple_impl(a, b, std::index_sequence_for<Rngs...>{});
    }
}
