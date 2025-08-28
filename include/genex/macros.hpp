#pragma once
#include <boost/preprocessor.hpp>


#define CHECK_TUPLE_EMPTY_PROBE(...) BOOST_PP_TUPLE_ELEM(0, 0, (__VA_ARGS__, ~))


#define IS_TUPLE_EMPTY(tuple) \
    BOOST_PP_IIF( \
        BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple), 1), \
        BOOST_PP_IS_EMPTY(CHECK_TUPLE_EMPTY_PROBE tuple), \
        0 \
    )


#define FORWARD_OBJECT_FOR_CLOSURE(_1, _2, obj) \
    obj = std::forward<decltype(obj)>(obj)


#define MAP_TO_BASE_SOLO_THIS_CAPTURE(...) \
    *this


#define FWD_CAPTURES(...) \
    BOOST_PP_IF(\
        IS_TUPLE_EMPTY(BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_THIS_CAPTURE,\
        BOOST_PP_SEQ_ENUM\
    )(BOOST_PP_SEQ_PUSH_BACK(\
        BOOST_PP_SEQ_TRANSFORM(FORWARD_OBJECT_FOR_CLOSURE, BOOST_PP_EMPTY, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)),\
        MAP_TO_BASE_SOLO_THIS_CAPTURE()\
    ))


#define RANGE_TO_ITERS(rng) iterators::begin(std::forward<decltype(rng)>(rng)), iterators::end(std::forward<decltype(rng)>(rng))


#define EXPORT_GENEX_STRUCT(name) \
    inline constexpr name ## _fn name
