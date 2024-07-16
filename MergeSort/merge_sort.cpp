#include <iostream>
#include <type_traits>

template <int... Args>
struct int_list {
    static constexpr int size = sizeof...(Args);
};

template <typename L, typename R>
struct type_pair {
    using lhs = L;
    using rhs = R;
};

template <int Pivot, int... Lhs, int... Rhs>
constexpr auto split(int_list<Lhs...>, int_list<Pivot, Rhs...>) {
    constexpr auto lhs_size = sizeof...(Lhs);
    constexpr auto rhs_size = sizeof...(Rhs);

    if constexpr (lhs_size < rhs_size) return split(int_list<Lhs..., Pivot>{}, int_list<Rhs...>{});
    else return type_pair<int_list<Lhs...>, int_list<Pivot, Rhs...>>{};
}

template <int L, int R, int... Lcs, int... Rcs, int... Result>
constexpr auto merge(int_list<Result...>, int_list<L, Lcs...>, int_list<R, Rcs...>) {
    if constexpr (L < R) return merge(int_list<Result..., L>{}, int_list<Lcs...>{}, int_list<R, Rcs...>{});
    else return merge(int_list<Result..., R>{}, int_list<L, Lcs...>{}, int_list<Rcs...>{});
}

template <int... Lcs, int... Result>
constexpr auto merge(int_list<Result...>, int_list<Lcs...>, int_list<>) {
    return int_list<Result..., Lcs...>{};
}

template <int... Rcs, int... Result>
constexpr auto merge(int_list<Result...>, int_list<>, int_list<Rcs...>) {
    return int_list<Result..., Rcs...>{};
}

template <int... Args>
constexpr auto merge_sort_impl(int_list<Args...>) {
    using list = int_list<Args...>;
    if constexpr (list::size <= 1) return list{};
    else {
        using split_result = decltype(split(int_list{}, list{}));
        using lhs = typename split_result::lhs;
        using rhs = typename split_result::rhs;
        return merge(int_list{}, merge_sort_impl(lhs{}), merge_sort_impl(rhs{}));
    }
}

template <int... Args>
using merge_sort = decltype(merge_sort_impl(int_list<Args...>{}));

template <int... Args>
void print(int_list<Args...>) {
    ((std::cout << Args << ' '), ...);
    std::cout << std::endl;
}

using sorted = merge_sort<3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5>;

int main() { print(sorted{}); }
