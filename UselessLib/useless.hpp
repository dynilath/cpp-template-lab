#include <type_traits>

namespace useless
{
    template<typename ...Ts>
    struct type_list{};

    namespace impl {
        template<template<typename ...> typename T, typename L>
        struct type_list_apply_type_impl;
        template<template<typename ...> typename T, typename ...Ts>
        struct type_list_apply_type_impl<T, type_list<Ts...>> : std::type_identity<T<Ts...>>{};
    }

    template<template<typename ...> typename T, typename L>
    using type_list_apply = typename impl::type_list_apply_type_impl<T, L>::type;

    namespace impl {
        template<typename L, typename T>
        struct contains_impl;
        template<typename T>
        struct contains_impl<type_list<>, T>: std::false_type{};
        template<typename L, typename ...Ls, typename T>
        struct contains_impl<type_list<L, Ls...>, T>: std::conditional_t<std::is_same_v<L, T>, std::true_type, contains_impl<type_list<Ls...>, T>>{};
    }

    template<typename L, typename T>
    inline constexpr bool type_list_contains = impl::contains_impl<L, T>::value;

    namespace impl {
        template<typename R, typename S>
        struct unique_type_list_impl;
        template<typename ...Rs>
        struct unique_type_list_impl<type_list<Rs...>,type_list<>> : std::type_identity<type_list<Rs...>>{};
        template<typename ...Rs, typename S, typename ...Ss>
        struct unique_type_list_impl<type_list<Rs...>, type_list<S, Ss...>> : std::conditional_t<
            type_list_contains<type_list<Rs...>, S>, 
            unique_type_list_impl<type_list<Rs...>, type_list<Ss...>>, 
            unique_type_list_impl<type_list<Rs..., S>, type_list<Ss...>>>{};
    }

    template<typename L>
    using unique_type_list = typename impl::unique_type_list_impl<type_list<>, L>::type;
}