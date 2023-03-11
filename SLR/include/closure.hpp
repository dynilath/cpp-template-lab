#pragma once
#include "./generation.hpp"

// is_item_in_item_set
// if the _item in question is in the _item_set, this type is derived from
// ::std::true_type otherwise it is derived from ::std::false_type
template <typename _item_set, typename _item> struct is_item_in_item_set;

template <typename _item>
struct is_item_in_item_set<::std::tuple<>, _item> : ::std::false_type {};

template <typename _Farg, typename _item>
struct is_item_in_item_set<::std::tuple<_Farg>, _item>
    : ::std::conditional_t<::std::is_same<_Farg, _item>::value,
                           ::std::true_type, ::std::false_type> {};

template <typename _Farg, typename... Args, typename _item>
struct is_item_in_item_set<::std::tuple<_Farg, Args...>, _item>
    : ::std::conditional_t<::std::is_same<_Farg, _item>::value,
                           ::std::true_type,
                           is_item_in_item_set<::std::tuple<Args...>, _item>> {
};

// create_itemset_from_symbol
//
template <typename _item_set_tuple, typename _Syn, typename _generations>
struct create_itemset_from_symbol;

template <typename... _items_in_set, typename _Syn>
struct create_itemset_from_symbol<::std::tuple<_items_in_set...>, _Syn,
                                  ::std::tuple<>> {
  using result = ::std::tuple<_items_in_set...>;
};

template <typename _Syn, typename _gen, typename... _generation>
struct create_itemset_from_symbol<::std::tuple<>, _Syn,
                                  ::std::tuple<_gen, _generation...>>
    : ::std::conditional_t<
          ::std::is_same<_Syn, typename _gen::generate_type>::value,
          create_itemset_from_symbol<
              ::std::tuple<typename create_item_with_first_dot<_gen>::result>,
              _Syn, ::std::tuple<_generation...>>,
          create_itemset_from_symbol<::std::tuple<>, _Syn,
                                     ::std::tuple<_generation...>>> {};

template <typename... _items_in_set, typename _Syn, typename _gen,
          typename... _generation>
struct create_itemset_from_symbol<::std::tuple<_items_in_set...>, _Syn,
                                  ::std::tuple<_gen, _generation...>>
    : ::std::conditional_t<
          ::std::is_same<_Syn, typename _gen::generate_type>::value &&
              !is_item_in_item_set<
                  ::std::tuple<_items_in_set...>,
                  typename create_item_with_first_dot<_gen>::result>::value,
          create_itemset_from_symbol<
              ::std::tuple<_items_in_set...,
                           typename create_item_with_first_dot<_gen>::result>,
              _Syn, ::std::tuple<_generation...>>,
          create_itemset_from_symbol<::std::tuple<_items_in_set...>, _Syn,
                                     ::std::tuple<_generation...>>> {};

// join_item_set
template <typename, typename> struct join_item_set;

template <typename... _items_in_A>
struct join_item_set<::std::tuple<_items_in_A...>, ::std::tuple<>> {
  using result = ::std::tuple<_items_in_A...>;
};

template <typename... _items_in_A, typename it_B, typename... items_in_B>
struct join_item_set<::std::tuple<_items_in_A...>,
                     ::std::tuple<it_B, items_in_B...>>
    : ::std::conditional_t<
          is_item_in_item_set<::std::tuple<_items_in_A...>, it_B>::value,
          join_item_set<::std::tuple<_items_in_A...>,
                        ::std::tuple<items_in_B...>>,
          join_item_set<::std::tuple<_items_in_A..., it_B>,
                        ::std::tuple<items_in_B...>>> {};

// lr_closure
// create closure of item set
template <typename _item_set_tuple, typename _target_item_set,
          typename _introduced_nonterminals, typename _generations>
struct lr_closure;

template <typename _generations>
struct lr_closure<::std::tuple<>, ::std::tuple<>, ::std::tuple<>,
                  _generations> {
  using result = ::std::tuple<>;
};

template <typename... _items_in_set, typename... _nters_introduced,
          typename _generations>
struct lr_closure<::std::tuple<_items_in_set...>, ::std::tuple<>,
                  ::std::tuple<_nters_introduced...>, _generations> {
  using result = ::std::tuple<_items_in_set...>;
};

template <typename _item, typename... _items, typename _generations>
struct lr_closure<::std::tuple<>, ::std::tuple<_item, _items...>,
                  ::std::tuple<>, _generations>
    : ::std::conditional_t<
          the_symbol_after_dot_is_nonterminal<_item>::value,
          lr_closure<
              ::std::tuple<_item>,
              typename create_itemset_from_symbol<
                  ::std::tuple<>, typename the_symbol_after_dot<_item>::type,
                  _generations>::result,
              ::std::tuple<>, _generations>,
          lr_closure<::std::tuple<_item>, ::std::tuple<_items...>,
                     ::std::tuple<>, _generations>> {};

template <typename... _items_in_set, typename _item, typename... _items,
          typename... _nters_introduced, typename _generations>
struct lr_closure<::std::tuple<_items_in_set...>,
                  ::std::tuple<_item, _items...>,
                  ::std::tuple<_nters_introduced...>, _generations>
    : ::std::conditional_t<
          the_symbol_after_dot_is_nonterminal<_item>::value &&
              !is_item_in_item_set<
                  ::std::tuple<_nters_introduced...>,
                  typename the_symbol_after_dot<_item>::type>::value,
          lr_closure<
              typename join_item_set<::std::tuple<_items_in_set...>,
                                     typename ::std::tuple<_item>>::result,
              typename join_item_set<
                  ::std::tuple<_items...>,
                  typename create_itemset_from_symbol<
                      ::std::tuple<>,
                      typename the_symbol_after_dot<_item>::type,
                      _generations>::result>::result,
              ::std::tuple<_nters_introduced...,
                           typename the_symbol_after_dot<_item>::type>,
              _generations>,
          lr_closure<
              typename join_item_set<::std::tuple<_items_in_set...>,
                                     typename ::std::tuple<_item>>::result,
              ::std::tuple<_items...>, ::std::tuple<_nters_introduced...>,
              _generations>> {};

// lr_goto
// create goto(I,X) from a closure
template <typename _lr_closure, typename _result, typename _Syn,
          typename _generations>
struct lr_goto;

template <typename... _results_in_set, typename _Syn, typename _generations>
struct lr_goto<::std::tuple<>, ::std::tuple<_results_in_set...>, _Syn,
               _generations> {
  using result =
      typename lr_closure<::std::tuple<>, ::std::tuple<_results_in_set...>,
                          ::std::tuple<>, _generations>::result;
};

template <typename _item, typename... _items_in_set, typename _Syn,
          typename _generations>
struct lr_goto<::std::tuple<_item, _items_in_set...>, ::std::tuple<>, _Syn,
               _generations>
    : ::std::conditional_t<
          ::std::is_same<_Syn,
                         typename the_symbol_after_dot<_item>::type>::value,
          lr_goto<::std::tuple<_items_in_set...>,
                  ::std::tuple<typename move_dot_ahead<_item>::result>, _Syn,
                  _generations>,
          lr_goto<::std::tuple<_items_in_set...>, ::std::tuple<>, _Syn,
                  _generations>> {};

template <typename _item, typename... _items_in_set,
          typename... _results_in_set, typename _Syn, typename _generations>
struct lr_goto<::std::tuple<_item, _items_in_set...>,
               ::std::tuple<_results_in_set...>, _Syn, _generations>
    : ::std::conditional_t<
          ::std::is_same<_Syn,
                         typename the_symbol_after_dot<_item>::type>::value,
          lr_goto<::std::tuple<_items_in_set...>,
                  ::std::tuple<_results_in_set...,
                               typename move_dot_ahead<_item>::result>,
                  _Syn, _generations>,
          lr_goto<::std::tuple<_items_in_set...>,
                  ::std::tuple<_results_in_set...>, _Syn, _generations>> {};

template <typename T> struct is_closures_empty : ::std::true_type {};

template <typename T, typename... Ts>
struct is_closures_empty<::std::tuple<T, Ts...>> : ::std::false_type {};

// lr_zero
// create LR(0)
template <typename _resultL, typename _resultR, typename _symbolL,
          typename _symbolR, typename _generations>
struct lr_zero;

template <typename... _closures_in_setL, typename... _symbol_in_R,
          typename _generations>
struct lr_zero<::std::tuple<_closures_in_setL...>, ::std::tuple<>,
               ::std::tuple<>, ::std::tuple<_symbol_in_R...>, _generations> {
  using result = ::std::tuple<_closures_in_setL...>;
};

template <typename... _closures_in_setL, typename closureT,
          typename... _closures_in_setR, typename... _symbol_in_L,
          typename _generations>
struct lr_zero<::std::tuple<_closures_in_setL...>,
               ::std::tuple<closureT, _closures_in_setR...>,
               ::std::tuple<_symbol_in_L...>, ::std::tuple<>, _generations>
    : lr_zero<::std::tuple<_closures_in_setL..., closureT>,
              ::std::tuple<_closures_in_setR...>, ::std::tuple<>,
              ::std::tuple<_symbol_in_L...>, _generations> {};

template <typename... _closures_in_setL, typename closureT,
          typename... _closures_in_setR, typename... _symbol_in_L,
          typename symbolT, typename... _symbol_in_R, typename _generations>
struct lr_zero<::std::tuple<_closures_in_setL...>,
               ::std::tuple<closureT, _closures_in_setR...>,
               ::std::tuple<_symbol_in_L...>,
               ::std::tuple<symbolT, _symbol_in_R...>, _generations>
    : ::std::conditional_t<
          is_closures_empty<typename lr_goto<closureT, ::std::tuple<>, symbolT,
                                             _generations>::result>::value ||
              is_item_in_item_set<
                  ::std::tuple<_closures_in_setL..., closureT,
                               _closures_in_setR...>,
                  typename lr_goto<closureT, ::std::tuple<>, symbolT,
                                   _generations>::result>::value,
          lr_zero<::std::tuple<_closures_in_setL...>,
                  ::std::tuple<closureT, _closures_in_setR...>,
                  ::std::tuple<_symbol_in_L..., symbolT>,
                  ::std::tuple<_symbol_in_R...>, _generations>,
          lr_zero<::std::tuple<_closures_in_setL...>,
                  ::std::tuple<closureT, _closures_in_setR...,
                               typename lr_goto<closureT, ::std::tuple<>,
                                                symbolT, _generations>::result>,
                  ::std::tuple<_symbol_in_L..., symbolT>,
                  ::std::tuple<_symbol_in_R...>, _generations>> {};