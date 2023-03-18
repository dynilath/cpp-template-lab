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

struct empty {};

template <typename T> struct result_type { using result = T; };

template <typename N>
using mark_Red = node<typename N::Left, typename N::Right, Red, N::value>;

template <typename N>
using mark_Black = node<typename N::Left, typename N::Right, Black, N::value>;

template <typename N, typename = void>
struct color_filp_single_node_impl : result_type<empty> {};
template <typename N>
struct color_filp_single_node_impl<N, std::void_t<typename N::color>> : result_type<
    std::conditional_t<std::is_same_v<typename N::color, Red>,
    node<typename N::Left, typename N::Right, Black, N::value>,
    node<typename N::Left, typename N::Right, Red, N::value>>> {};
template <typename N> using cfilp_single = color_filp_single_node_impl<N>::result;

template <typename N, typename = void> struct is_Red : std::false_type {};
template <typename N> struct is_Red<N, std::void_t<typename N::color>> : std::conditional_t<std::is_same_v<typename N::color, Red>,
    std::true_type, std::false_type> {};
template <typename N> inline static constexpr auto is_Red_v = is_Red<N>::value;

// rotateLeft
template <typename N, typename = void> struct rotateLeft_impl : result_type<N> {};
template <typename N>
struct rotateLeft_impl<N, std::void_t<typename N::Right::Left>> : result_type<
    node<node<typename N::Left, typename N::Right::Left, Red, N::value>,
    typename N::Right::Right,
    typename N::color,
    N::Right::value>> {};
template <typename N> using rotateLeft = rotateLeft_impl<N>::result;

// rotateRight
template <typename N, typename = void> struct rotateRight_impl : result_type<N> {};
template <typename N>
struct rotateRight_impl<N, std::void_t<typename N::Left::Right>> : result_type<
    node<typename N::Left::Left,
    node<typename N::Left::Right, typename N::Right, Red, N::value>,
    typename N::color,
    N::Left::value>> {};
template <typename N> using rotateRight = rotateRight_impl<N>::result;

// colorFlip
template <typename N, typename = void>
struct colorFlip_impl : result_type<empty> {};
template <typename N>
struct colorFlip_impl<N, std::void_t<typename N::color>> : result_type<cfilp_single<
    node<cfilp_single<typename N::Left>, cfilp_single<typename N::Right>, typename N::color, N::value>>> {};
template <typename N> using colorFlip = colorFlip_impl<N>::result;

namespace LLRB_balance {
    // if left is not red and right is red, do rotateLeft
    template <typename N>
    struct rebalance_impl_1 : std::conditional_t<(!is_Red_v<typename N::Left>&& is_Red_v<typename N::Right>), result_type<rotateLeft<N>>, result_type<N>> {};
    template<> struct rebalance_impl_1<empty> : result_type<empty> {};

    // if left is red and left of left is red, do rotateRight
    template <typename N, typename = void> struct l_red_ll_red : std::false_type {};
    template <typename N>
    struct l_red_ll_red<N, std::void_t<typename N::Left::Left>> : std::bool_constant<(is_Red_v<typename N::Left>&& is_Red_v<typename N::Left::Left>)> {};
    template <typename N>
    struct rebalance_impl_2 : result_type<std::conditional_t<l_red_ll_red<N>::value, rotateRight<N>, N>> {};
    template<> struct rebalance_impl_2<empty> : result_type<empty> {};

    // if left is red and right is red, do flipColors
    template <typename N>
    struct rebalance_impl_3 : std::conditional_t<(is_Red_v<typename N::Left>&& is_Red_v<typename N::Right>), result_type<colorFlip<N>>, result_type<N>> {};
    template<> struct rebalance_impl_3<empty> : result_type<empty> {};

    template <typename N>
    using rebalance = typename rebalance_impl_3<typename rebalance_impl_2<typename rebalance_impl_1<N>::result>::result>::result;
}

namespace LLRB_insert {
    template <typename H, unsigned V, typename = void>
    struct insert_impl;
    template <unsigned V>
    struct insert_impl<empty, V> : result_type<node<empty, empty, Red, V>> {};
    template <typename H, unsigned V>
    struct insert_impl<H, V, std::void_t<decltype(H::value), typename H::Right, typename H::Left>> : result_type<
        std::conditional_t<(V == H::value), H, LLRB_balance::rebalance<std::conditional_t<(V < H::value),
        node<typename insert_impl<typename H::Left, V>::result, typename H::Right, typename H::color, H::value>,
        node<typename H::Left, typename insert_impl<typename H::Right, V>::result, typename H::color, H::value>>>>> {};
}

namespace LLRB_erase {
    template<typename N, typename = void>
    struct moveRedLeft_step_2 : result_type<N> {};
    template<typename N>
    struct moveRedLeft_step_2<N, std::void_t<typename N::Right::Left>> : result_type<std::conditional_t<is_Red_v<typename N::Right::Left>,
        colorFlip<rotateLeft<node<typename N::Left, rotateRight<typename N::Right>, typename N::color, N::value>>>, N>> {};
    template <typename N, typename = void>
    struct moveRedLeft_impl : result_type<typename moveRedLeft_step_2<colorFlip<N>>::result> {};
    template <typename N> using moveRedLeft = typename moveRedLeft_impl<N>::result;

    template<typename N, typename = void>
    struct moveRedRight_step_2 : result_type<N> {};
    template<typename N>
    struct moveRedRight_step_2<N, std::void_t<typename N::Left::Left>> : result_type<std::conditional_t<is_Red_v<typename N::Left::Left>,
        colorFlip<rotateRight<N>>, N>> {};
    template <typename N, typename = void>
    struct moveRedRight_impl : result_type<typename moveRedRight_step_2<colorFlip<N>>::result> {};
    template <typename N> using moveRedRight = typename moveRedRight_impl<N>::result;

    template <typename N, typename = void>
    struct neither_l_red_ll_red : std::false_type {};
    template <typename N>
    struct neither_l_red_ll_red<N, std::void_t<typename N::Left::Left>> : std::bool_constant<(!is_Red_v<typename N::Left> && !is_Red_v<typename N::Left::Left>)> {};
    template<typename N>
    struct moveRedLeft_when_neither_l_ll_red_impl : result_type<std::conditional_t<neither_l_red_ll_red<N>::value, moveRedLeft<N>, N>> {};
    template<>
    struct moveRedLeft_when_neither_l_ll_red_impl<empty> : result_type<empty> {};
    template<typename N>
    using moveRedLeft_when_neither_l_ll_red = typename moveRedLeft_when_neither_l_ll_red_impl<N>::result;

    template<template <typename> typename Erase_, typename H, typename = void>
    struct eraseMinLeft_impl : result_type<H> {};
    template<template <typename> typename Erase_, typename H>
    struct eraseMinLeft_impl<Erase_, H, std::void_t<typename H::Left>> : result_type<
        node<typename Erase_<typename H::Left>::result, typename H::Right, typename H::color, H::value>> {};
    template<template <typename> typename Erase_, typename H>
    using erase_min_left = typename eraseMinLeft_impl<Erase_, H>::result;

    template<typename H, typename = void>
    struct eraseMin_impl : result_type<H> {};
    template<typename H>
    struct eraseMin_impl<H, std::void_t<typename H::Left>> : result_type<std::conditional_t<std::is_same_v<typename H::Left, empty>,
        empty, LLRB_balance::rebalance<erase_min_left<eraseMin_impl, moveRedLeft_when_neither_l_ll_red<H>>>>> {};
    template<typename H>
    using eraseMin = typename eraseMin_impl<H>::result;

    template<template <typename, unsigned> typename Erase_, typename H, unsigned V, typename = void>
    struct eraseLeft_impl : result_type<H> {};
    template<template <typename, unsigned> typename Erase_, typename H, unsigned V>
    struct eraseLeft_impl<Erase_, H, V, std::void_t<typename H::Left>> : result_type<
        node<typename Erase_<typename H::Left, V>::result, typename H::Right, typename H::color, H::value>> {};
    template<template <typename, unsigned> typename Erase_, typename H, unsigned V>
    using erase_left = typename eraseLeft_impl<Erase_, H, V>::result;

    template<typename N, typename = void>
    struct erase_impl_2_step1 : result_type<N> {};
    template<typename N>
    struct erase_impl_2_step1<N, std::void_t<typename N::Left>> : result_type<
        std::conditional_t<is_Red_v<typename N::Left>, rotateRight<N>, N>> {};

    template<template <typename> typename EraseMin_, typename N, typename = void>
    struct absorb_right_impl : result_type<N> {};
    template<template <typename> typename EraseMin_, typename N>
    struct absorb_right_impl <EraseMin_, N, std::void_t<decltype(N::Right::value)>> : result_type<
        node<typename N::Left, EraseMin_<typename N::Right>, typename N::Right::color, N::Right::value>> {};
    template<template <typename> typename EraseMin_, typename N>
    using absorb_right = typename absorb_right_impl<EraseMin_, N>::result;

    template<template <typename, unsigned> typename Erase_, typename N, unsigned V, typename = void>
    struct erase_impl_2_step2 : result_type<N> {};
    template<template <typename, unsigned> typename Erase_, typename N, unsigned V>
    struct erase_impl_2_step2<Erase_, N, V, std::void_t<decltype(N::value)>> :
        result_type<std::conditional_t<(N::value == V),
        absorb_right<eraseMin, N>,
        node<typename N::Left, typename Erase_<typename N::Right, V>::result, typename N::color, N::value>
        >> {};

    template <typename N, typename = void>
    struct neither_r_red_rl_red : std::false_type {};
    template <typename N>
    struct neither_r_red_rl_red<N, std::void_t<typename N::Right::Left>> : std::bool_constant<(!is_Red_v<typename N::Right> && !is_Red_v<typename N::Right::Left>)> {};
    template<typename N>
    struct moveRedRight_when_neither_r_rl_red_impl : result_type<std::conditional_t<neither_r_red_rl_red<N>::value, moveRedLeft<N>, N>> {};
    template<>
    struct moveRedRight_when_neither_r_rl_red_impl<empty> : result_type<empty> {};
    template<typename N>
    using moveRedRight_when_neither_r_rl_red = typename moveRedRight_when_neither_r_rl_red_impl<N>::result;

    template<template <typename, unsigned> typename Erase_, typename N, unsigned V, typename = void>
    struct erase_impl_2_step3 : result_type<N> {};
    template<template <typename, unsigned> typename Erase_, typename N, unsigned V>
    struct erase_impl_2_step3<Erase_, N, V, std::void_t<typename N::Right, decltype(N::value)>> : result_type<
        std::conditional_t<(N::value == V && std::is_same_v<typename N::Right, empty>),
        empty,
        typename erase_impl_2_step2<Erase_, moveRedRight_when_neither_r_rl_red<N>, V>::result
        >>{};

    template <typename H, unsigned V, typename = void> struct erase_impl;
    template <unsigned V> struct erase_impl<empty, V> : result_type<empty> {};
    template <typename H, unsigned V>
    struct erase_impl<H, V, std::void_t<decltype(H::value)>> : result_type<
        LLRB_balance::rebalance<std::conditional_t<(V < H::value),
        erase_left<erase_impl, moveRedLeft_when_neither_l_ll_red<H>, V>,
        typename erase_impl_2_step3<erase_impl, H, V>::result
        >>> {};
}

template <typename root_> struct LLRB { using root = root_; };

template <typename T, unsigned V>
using insert = LLRB<mark_Black<typename LLRB_insert::insert_impl<typename T::root, V>::result>>;

template <typename T, unsigned V>
using erase = LLRB<mark_Black<typename LLRB_erase::erase_impl<typename T::root, V>::result>>;

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
void print_node(const node<L, R, RB, V>& n, int depth = 0) {
    if constexpr (std::is_same_v<RB, Red>) std::cout << "R:";
    else std::cout << "B:";
    std::cout << V;
    if constexpr (!std::is_same_v<L, empty>) std::cout << " (", print_node(L{}, depth + 1), std::cout << ")";
    else std::cout << " ()";
    if constexpr (!std::is_same_v<R, empty>) std::cout << " (", print_node(R{}, depth + 1), std::cout << ")";
    else std::cout << " ()";
}

using LLRB0 = LLRB<empty>;
using LLRB1 = list_insert<LLRB0, 1, 2, 4, 3, 6, 9, 11, 12>;
using LLRB2 = erase<LLRB1, 3>;
using LLRB3 = erase<LLRB2, 5>;
using LLRB4 = erase<LLRB3, 9>;

int main() {
    print_node(LLRB1::root{});
    std::cout << "\n";
    print_node(LLRB2::root{});
    std::cout << "\n";
    print_node(LLRB3::root{});
    std::cout << "\n";
    print_node(LLRB4::root{});
}