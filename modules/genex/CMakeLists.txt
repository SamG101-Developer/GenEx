add_library(GenEx_All STATIC)

target_sources(GenEx_All PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES
        ${CMAKE_CURRENT_LIST_DIR}/actions/_action_base.ixx

        ${CMAKE_CURRENT_LIST_DIR}/algorithms/_algorithm_base.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/accumulate.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/all_of.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/any_of.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/binary_search.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/contains.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/count.ixx
        ${CMAKE_CURRENT_LIST_DIR}/algorithms/find.ixx

        ${CMAKE_CURRENT_LIST_DIR}/iterators/advance.ixx
        ${CMAKE_CURRENT_LIST_DIR}/iterators/begin.ixx
        ${CMAKE_CURRENT_LIST_DIR}/iterators/distance.ixx
        ${CMAKE_CURRENT_LIST_DIR}/iterators/end.ixx
        ${CMAKE_CURRENT_LIST_DIR}/iterators/next.ixx

        ${CMAKE_CURRENT_LIST_DIR}/operations/insert.ixx
        ${CMAKE_CURRENT_LIST_DIR}/operations/push_back.ixx
        ${CMAKE_CURRENT_LIST_DIR}/operations/push_front.ixx
        ${CMAKE_CURRENT_LIST_DIR}/operations/size.ixx

        ${CMAKE_CURRENT_LIST_DIR}/views/_view_base.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/chunk.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/concat.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/copied.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/cycle.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/drop.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/enumerate.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/filter.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/flat.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/fold.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/for_each.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/interleave.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/intersperse.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/iota.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/map.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/remove.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/replace.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/reverse.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/set_algorithms.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/slice.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/take.ixx
        ${CMAKE_CURRENT_LIST_DIR}/views/to.ixx

        ${CMAKE_CURRENT_LIST_DIR}/concepts.ixx
        ${CMAKE_CURRENT_LIST_DIR}/generator.ixx
        ${CMAKE_CURRENT_LIST_DIR}/meta.ixx
        ${CMAKE_CURRENT_LIST_DIR}/type_traits.ixx)

include_directories(${CMAKE_SOURCE_DIR}/headers)

add_library(GenEx::All ALIAS GenEx_All)
