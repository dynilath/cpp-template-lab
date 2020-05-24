#pragma once

#include <tuple>
#include <iostream>

#include "./generation.hpp"

template<typename it>
void printf_tuple(const ::std::tuple<it>&){
    ::std::cout << typeid(it).name() << std::endl;
}

template<typename it,typename ...items>
void printf_tuple(const ::std::tuple<it,items...>&){
    ::std::cout << typeid(it).name() << " ";
    printf_tuple(::std::tuple<items...>());
}

template<typename it>
void printf_generation(const generation<it>&){
    ::std::cout << typeid(it).name() << std::endl;
}

template<typename it,typename ...items>
void printf_generation(const generation<it,items...>&){
    ::std::cout << typeid(it).name() << " ";
    printf_generation(generation<items...>());
}

void print_closure(const ::std::tuple<>&){
    ::std::endl(::std::cout);
}

template<typename it,typename...items>
void print_closure(const ::std::tuple<it,items...>&){
    printf_generation(it());
    print_closure(::std::tuple<items...>());
}

void print_closure_set_impl(const ::std::tuple<>&,size_t i){
    ::std::cout << ::std::endl;
}
template<typename it,typename...items>
void print_closure_set_impl(const ::std::tuple<it,items...>&,size_t i){
    ::std::cout << "L" << i << ::std::endl;
    print_closure(it());
    print_closure_set_impl(::std::tuple<items...>(),i+1);
}

template<typename...items>
void print_closure_set(const ::std::tuple<items...>&){
    print_closure_set_impl(::std::tuple<items...>(),0);
}