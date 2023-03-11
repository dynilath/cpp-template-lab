# Dijkstra in C++ templates

```C++
struct V1 {};
struct V2 {};
struct V3 {};
struct V4 {};
struct V5 {};
struct V6 {};

using E1 = Edge<V1, V2, 7>;
using E2 = Edge<V1, V3, 14>;
using E3 = Edge<V1, V4, 9>;
using E4 = Edge<V2, V4, 10>;
using E5 = Edge<V2, V5, 15>;
using E6 = Edge<V3, V6, 9>;
using E7 = Edge<V3, V4, 2>;
using E8 = Edge<V4, V5, 11>;
using E9 = Edge<V5, V6, 6>;

using Vs = std::tuple<V1, V2, V3, V4, V5, V6>;
using Es = std::tuple<E1, E2, E3, E4, E5, E6, E7, E8, E9>;
using solved = dijkstra_solve<Vs, Es, V1>;

int main() {
  std::cout << "V1 distance to V1 : " << solved::target<V1>::value << "\n";
  std::cout << "V2 distance to V1 : " << solved::target<V2>::value << "\n";
  std::cout << "V3 distance to V1 : " << solved::target<V3>::value << "\n";
  std::cout << "V4 distance to V1 : " << solved::target<V4>::value << "\n";
  std::cout << "V5 distance to V1 : " << solved::target<V5>::value << "\n";
  std::cout << "V6 distance to V1 : " << solved::target<V6>::value << "\n";
}
```

output:
```
V1 distance to V1 : 0
V2 distance to V1 : 7
V3 distance to V1 : 11
V4 distance to V1 : 9
V5 distance to V1 : 20
V6 distance to V1 : 20
```