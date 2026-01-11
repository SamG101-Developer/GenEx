export module genex;

// Core modules
export import genex.concepts;
export import genex.generator;
export import genex.meta;
export import genex.pipe;
export import genex.span;
export import genex.to_container;

// Actions
export import genex.actions.clear;
export import genex.actions.concat;
export import genex.actions.drop;
export import genex.actions.drop_while;
export import genex.actions.erase;
export import genex.actions.insert;
export import genex.actions.pop_back;
export import genex.actions.pop_front;
export import genex.actions.push_back;
export import genex.actions.push_front;
export import genex.actions.remove;
export import genex.actions.remove_if;
export import genex.actions.replace;
export import genex.actions.replace_if;
export import genex.actions.reverse;
export import genex.actions.shuffle;
export import genex.actions.slice;
export import genex.actions.sort;
export import genex.actions.take;
export import genex.actions.take_while;

// Algorithms
export import genex.algorithms.all_of;
export import genex.algorithms.any_of;
export import genex.algorithms.binary_search;
export import genex.algorithms.concepts;
export import genex.algorithms.contains;
export import genex.algorithms.contains_if;
export import genex.algorithms.count;
export import genex.algorithms.count_if;
export import genex.algorithms.equals;
export import genex.algorithms.find;
export import genex.algorithms.find_if;
export import genex.algorithms.find_if_not;
export import genex.algorithms.find_last;
export import genex.algorithms.find_last_if;
export import genex.algorithms.find_last_if_not;
export import genex.algorithms.fold_left;
export import genex.algorithms.fold_left_first;
export import genex.algorithms.fold_right;
export import genex.algorithms.fold_right_first;
export import genex.algorithms.max_element;
export import genex.algorithms.min_element;
export import genex.algorithms.none_of;
export import genex.algorithms.position;
export import genex.algorithms.position_last;
export import genex.algorithms.sorted;
export import genex.algorithms.tuple;

// Iterators
export import genex.iterators.access;
export import genex.iterators.advance;
export import genex.iterators.distance;
export import genex.iterators.iter_pair;
export import genex.iterators.next;
export import genex.iterators.prev;

// Operations
export import genex.operations.access;
export import genex.operations.cmp;
export import genex.operations.empty;
export import genex.operations.size;

// Strings
export import genex.strings.cases;

// Views
export import genex.views2.address_of;
export import genex.views2.cast_dynamic;
export import genex.views2.cast_smart;
export import genex.views2.cast_static;
export import genex.views2.chunk;
export import genex.views2.concat;
export import genex.views2.cycle;
export import genex.views2.drop;
export import genex.views2.drop_last;
export import genex.views2.drop_while;
export import genex.views2.duplicates;
export import genex.views2.enumerate;
export import genex.views2.filter;
export import genex.views2.for_each;
export import genex.views2.indirect;
export import genex.views2.interleave;
export import genex.views2.intersperse;
export import genex.views2.iota;
export import genex.views2.join;
export import genex.views2.join_with;
export import genex.views2.map;
export import genex.views2.move;
export import genex.views2.move_reverse;
export import genex.views2.ptr;
export import genex.views2.remove;
export import genex.views2.remove_if;
export import genex.views2.replace;
export import genex.views2.replace_if;
export import genex.views2.reverse;
export import genex.views2.set_algorithms;
export import genex.views2.slice;
export import genex.views2.split;
export import genex.views2.take;
export import genex.views2.take_last;
export import genex.views2.take_while;
export import genex.views2.transform;
export import genex.views2.tuple_nth;
export import genex.views2.view;
export import genex.views2.zip;
