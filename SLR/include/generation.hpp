#pragma once
#include <tuple>
#include <type_traits>

struct parse_unit {};

struct terminal : parse_unit {};

struct nonterminal : parse_unit {};

struct dot_placeholder : parse_unit {};

struct epsilon : parse_unit {};

template <typename _arg1, typename... _Args> struct generation {
  using generate_type = _arg1;
};

template <typename _type> struct type_holder {
  using type = _type;
};

template <typename _item, typename... _Args> struct fist_arg_type_holder {
  using type = _item;
};

template <typename _item> struct the_symbol_after_dot;

template <typename _Lhs, typename _Syb, typename... _Args>
struct the_symbol_after_dot<generation<_Lhs, _Syb, _Args...>>
    : ::std::conditional_t<::std::is_same<_Syb, dot_placeholder>::value,
                           fist_arg_type_holder<_Args...>,
                           the_symbol_after_dot<generation<_Lhs, _Args...>>> {};

template <typename _Lhs, typename _Syb>
struct the_symbol_after_dot<generation<_Lhs, _Syb>> : type_holder<epsilon> {};

template <typename> struct create_item_with_first_dot;

template <typename _Gen, typename... _Symbols>
struct create_item_with_first_dot<generation<_Gen, _Symbols...>> {
  using result = generation<_Gen, dot_placeholder, _Symbols...>;
};

template <typename _item>
using the_symbol_after_dot_is_nonterminal =
    ::std::is_base_of<nonterminal, typename the_symbol_after_dot<_item>::type>;

template <typename, typename> struct move_dot_ahead_result;
template <typename... Ls, typename V, typename... Rs>
struct move_dot_ahead_result<::std::tuple<Ls...>, ::std::tuple<V, Rs...>> {
  using result = generation<Ls..., V, dot_placeholder, Rs...>;
};
template <typename... Ls, typename V>
struct move_dot_ahead_result<::std::tuple<Ls...>, ::std::tuple<V>> {
  using result = generation<Ls..., V, dot_placeholder>;
};
template <typename... Ls>
struct move_dot_ahead_result<::std::tuple<Ls...>, ::std::tuple<>> {
  using result = generation<Ls..., dot_placeholder>;
};

template <typename, typename> struct move_dot_ahead_helper;

template <typename... Ls, typename V, typename... Rs>
struct move_dot_ahead_helper<::std::tuple<Ls...>, ::std::tuple<V, Rs...>>
    : ::std::conditional_t<
          ::std::is_same<V, dot_placeholder>::value,
          move_dot_ahead_result<::std::tuple<Ls...>, ::std::tuple<Rs...>>,
          move_dot_ahead_helper<::std::tuple<Ls..., V>, ::std::tuple<Rs...>>> {
};

template <typename _generation_T> struct move_dot_ahead;

template <typename _arg1, typename... _Args>
struct move_dot_ahead<generation<_arg1, _Args...>>
    : move_dot_ahead_helper<::std::tuple<_arg1>, ::std::tuple<_Args...>> {};