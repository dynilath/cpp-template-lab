# LLRB in C++ templates

```C++
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
```

output:
```
B:4 (B:2 (B:1 () ()) (B:3 () ())) (B:8 (B:7 (R:5 () ()) ()) (B:9 () ()))
B:8 (R:4 (B:2 (B:1 () ()) (B:3 () ())) (B:6 (B:5 () ()) (B:7 () ()))) (B:11 (B:9 () ()) (B:25 (R:22 () ()) ()))
```