#ifndef MACROS_HPP
#define MACROS_HPP

#include <boost/preprocessor.hpp>


#define CHECK_TUPLE_EMPTY_PROBE(...) BOOST_PP_TUPLE_ELEM(0, 0, (__VA_ARGS__, ~))
#define IS_TUPLE_EMPTY(tuple) \
    BOOST_PP_IIF( \
        BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple), 1), \
        BOOST_PP_IS_EMPTY(CHECK_TUPLE_EMPTY_PROBE tuple), \
        0 \
    )

#define FORWARD_OBJECT(_1, _2, obj) \
    std::forward<decltype(obj)>(obj)

#define FORWARD_OBJECT_FOR_CLOSURE(_1, _2, obj) \
    obj = std::forward<decltype(obj)>(obj)

#define SPLIT_RANGE_TO_ITER(_1, _2, i, range) \
    BOOST_PP_COMMA_IF(i) iterators::begin(range), iterators::end(range)

#define MAP_TO_IMPL(fn, ...) \
    return fn(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))))


#define MAP_TO_BASE_SOLO_THIS_CAPTURE(...) \
    this

#define MAP_TO_BASE_CAPTURES(...) \
    BOOST_PP_IF(\
        IS_TUPLE_EMPTY(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_THIS_CAPTURE,\
        BOOST_PP_SEQ_ENUM\
    )(BOOST_PP_SEQ_PUSH_BACK(\
        BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT_FOR_CLOSURE, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_THIS_CAPTURE()\
    ))

#define MAP_TO_BASE_SOLO_FORWARD_RANGE_ARGUMENT(...) \
    std::forward<Rng>(rng)

#define MAP_TO_BASE_ARGUMENTS(...) \
    BOOST_PP_IF(\
        IS_TUPLE_EMPTY(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_FORWARD_RANGE_ARGUMENT,\
        BOOST_PP_SEQ_ENUM\
    )(BOOST_PP_SEQ_PUSH_FRONT(\
        BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_FORWARD_RANGE_ARGUMENT()\
    ))

#define MAP_TO_BASE(...) \
    return [MAP_TO_BASE_CAPTURES(__VA_ARGS__)]<range Rng>(Rng &&rng) mutable -> decltype(auto) { \
        return (*this)(MAP_TO_BASE_ARGUMENTS(__VA_ARGS__)); \
    }

#define EXPORT_GENEX_STRUCT(name) \
    inline constexpr name ## _fn name


#endif //MACROS_HPP
