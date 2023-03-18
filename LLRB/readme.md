# LLRB in C++ templates

```C++
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
```

output:
```
B:4 (B:2 (B:1 () ()) (B:3 () ())) (B:9 (B:6 () ()) (B:12 (R:11 () ()) ()))
B:9 (R:4 (B:2 (R:1 () ()) ()) (B:6 () ())) (B:12 (R:11 () ()) ())
B:9 (R:2 (B:1 () ()) (B:4 () ())) (B:12 (R:11 () ()) ())
B:12 (R:2 (B:1 () ()) (B:4 () ())) (B:12 () ())
```