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
export import genex.views.address;
export import genex.views.borrow;
export import genex.views.cast_dynamic;
export import genex.views.cast_smart;
export import genex.views.cast_static;
export import genex.views.chunk;
export import genex.views.concat;
export import genex.views.cycle;
export import genex.views.drop;
export import genex.views.drop_last;
export import genex.views.drop_while;
export import genex.views.duplicates;
export import genex.views.enumerate;
export import genex.views.filter;
export import genex.views.for_each;
export import genex.views.indirect;
export import genex.views.interleave;
export import genex.views.intersperse;
export import genex.views.iota;
export import genex.views.join;
export import genex.views.join_with;
export import genex.views.map;
export import genex.views.materialize;
export import genex.views.move;
export import genex.views.move_reverse;
export import genex.views.ptr;
export import genex.views.remove;
export import genex.views.remove_if;
export import genex.views.replace;
export import genex.views.replace_if;
export import genex.views.reverse;
export import genex.views.sample;
export import genex.views.set_algorithms;
export import genex.views.slice;
export import genex.views.split;
export import genex.views.take;
export import genex.views.take_last;
export import genex.views.take_while;
export import genex.views.transform;
export import genex.views.tuple_nth;
export import genex.views.view;
export import genex.views.zip;


