#include <iostream>
#include <type_traits>

struct Red {};
struct Black {};

template <typename L, typename R, typename RB_, unsigned V> struct node {
  using Left = L;
  using Right = R;
  using color = RB_;
  static constexpr auto value = V;
};

template <typename T> struct result_type {
  using result = T;
};
struct empty {};

template <typename n>
using mark_Red = node<typename n::Left, typename n::Right, Red, n::value>;

template <typename n>
using mark_Black = node<typename n::Left, typename n::Right, Black, n::value>;

template <typename n, typename = void>
struct mark_Flip_impl : result_type<empty> {};
template <typename n>
struct mark_Flip_impl<n, std::void_t<typename n::color>>
    : result_type<std::conditional_t<
          std::is_same_v<typename n::color, Red>,
          node<typename n::Left, typename n::Right, Black, n::value>,
          node<typename n::Left, typename n::Right, Red, n::value>>> {};
template <typename n> using mark_Flip = mark_Flip_impl<n>::result;

template <typename n, typename = void> struct is_Red : std::false_type {};
template <typename n>
struct is_Red<n, std::void_t<typename n::color>>
    : std::conditional_t<std::is_same_v<typename n::color, Red>, std::true_type,
                         std::false_type> {};
template <typename n> inline static constexpr auto is_Red_v = is_Red<n>::value;

template <typename n, typename = void> struct rotL_impl : result_type<n> {};
template <typename n>
struct rotL_impl<n, std::void_t<typename n::Right::Left>>
    : result_type<
          node<node<typename n::Left, typename n::Right::Left, Red, n::value>,
               typename n::Right::Right, typename n::color, n::Right::value>> {
};
template <typename n> using rotL = rotL_impl<n>::result;

template <typename n, typename = void> struct rotR_impl : result_type<n> {};
template <typename n>
struct rotR_impl<n, std::void_t<typename n::Left::Right>>
    : result_type<
          node<typename n::Left::Left,
               node<typename n::Left::Right, typename n::Right, Red, n::value>,
               typename n::color, n::Left::value>> {};
template <typename n> using rotR = rotR_impl<n>::result;

template <typename n, typename = void>
struct colorFlip_impl : result_type<empty> {};
template <typename n>
struct colorFlip_impl<n, std::void_t<typename n::color>>
    : result_type<mark_Flip<
          node<mark_Flip<typename n::Left>, mark_Flip<typename n::Right>,
               typename n::color, n::value>>> {};
template <typename n> using colorFlip = colorFlip_impl<n>::result;

template <typename n>
struct LLRB_balance_impl_1
    : std::conditional_t<(!is_Red_v<typename n::Left> &&
                          is_Red_v<typename n::Right>),
                         result_type<rotL<n>>, result_type<n>> {};

template <typename n, typename = void> struct l_red_ll_red : std::false_type {};
template <typename n>
struct l_red_ll_red<n, std::void_t<typename n::Left::Left>>
    : std::conditional_t<(is_Red_v<typename n::Left> &&
                          is_Red_v<typename n::Left::Left>),
                         std::true_type, std::false_type> {};
template <typename n>
struct LLRB_balance_impl_2
    : std::conditional_t<l_red_ll_red<n>::value, result_type<rotR<n>>,
                         result_type<n>> {};
template <typename n>
struct LLRB_balance_impl_3
    : std::conditional_t<(is_Red_v<typename n::Left> &&
                          is_Red_v<typename n::Right>),
                         result_type<colorFlip<n>>, result_type<n>> {};
template <typename n>
using LLRB_balance = typename LLRB_balance_impl_3<typename LLRB_balance_impl_2<
    typename LLRB_balance_impl_1<n>::result>::result>::result;

template <typename H, unsigned V, typename = void> struct LLRB_insert;
template <unsigned V>
struct LLRB_insert<empty, V> : result_type<node<empty, empty, Red, V>> {};
template <typename H, unsigned V>
struct LLRB_insert<
    H, V, std::void_t<decltype(H::value), typename H::Right, typename H::Left>>
    : result_type<std::conditional_t<
          (V == H::value), H,
          LLRB_balance<std::conditional_t<
              (V < H::value),
              node<typename LLRB_insert<typename H::Left, V>::result,
                   typename H::Right, typename H::color, H::value>,
              node<typename H::Left,
                   typename LLRB_insert<typename H::Right, V>::result,
                   typename H::color, H::value>>>>> {};

template <typename root_> struct LLRB {
  using root = root_;

  template <unsigned V>
  using insert = LLRB<mark_Black<typename LLRB_insert<root, V>::result>>;
};

template <typename T, unsigned V>
using insert =
    LLRB<mark_Black<typename LLRB_insert<typename T::root, V>::result>>;

template <unsigned... Is> struct i_sequence {};

template <typename T, typename C> struct list_insert_impl;
template <typename T>
struct list_insert_impl<T, i_sequence<>> : result_type<T> {};
template <typename T, unsigned I, unsigned... Is>
struct list_insert_impl<T, i_sequence<I, Is...>>
    : list_insert_impl<insert<T, I>, i_sequence<Is...>> {};
template <typename T, unsigned... Is>
using list_insert = list_insert_impl<T, i_sequence<Is...>>::result;

// dfs
template <typename L, typename R, typename RB, unsigned V>
void print_node(const node<L, R, RB, V> &n, int depth = 0) {
  if constexpr (std::is_same_v<RB, Red>)
    std::cout << "R:";
  else
    std::cout << "B:";
  std::cout << V;
  if constexpr (!std::is_same_v<L, empty>)
    std::cout << " (", print_node(L{}, depth + 1), std::cout << ")";
  else
    std::cout << " ()";
  if constexpr (!std::is_same_v<R, empty>)
    std::cout << " (", print_node(R{}, depth + 1), std::cout << ")";
  else
    std::cout << " ()";
}

using LLRB0 = LLRB<empty>;
using LLRB1 = list_insert<LLRB0, 1, 2, 4, 3, 5, 9, 8, 7>;
using LLRB2 = list_insert<LLRB1, 6, 11, 22, 25>;

int main() {
  print_node(LLRB1::root{});
  std::cout << "\n";
  print_node(LLRB2::root{});
}