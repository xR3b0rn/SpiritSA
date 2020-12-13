#pragma once

template <std::size_t I, class T>
auto get(const T& t)
{
  return boost::pfr::get<I>(t);
}
template <std::size_t I, class... Args>
auto get(const std::tuple<Args...>& t)
{
  return std::get<I>(t);
}
template <class T>
auto tie(T& t)
{
  return boost::pfr::structure_tie(t);
}
template <class... Args>
auto& tie(std::tuple<Args...>& t)
{
  return t;
}
template <class T>
struct tuple_size
{
  static constexpr std::size_t value = boost::pfr::tuple_size_v<T>;
};
template <class... Args>
struct tuple_size<std::tuple<Args...>>
{
  static constexpr std::size_t value = sizeof...(Args);
};
template <class T>
constexpr std::size_t tuple_size_v = tuple_size<T>::value;
template <class T1, class T2>
struct tuple_cat;
template <class... Args1, class... Args2>
struct tuple_cat<std::tuple<Args1...>, std::tuple<Args2...>>
{
  using type = std::tuple<Args1..., Args2...>;
};
template <class T1, class T2>
using tuple_cat_t = typename tuple_cat<T1, T2>::type;
template <class T1, class T2>
struct tuple_cat_rmr;
template <class... Args1, class... Args2>
struct tuple_cat_rmr<std::tuple<Args1...>, std::tuple<Args2...>>
{
  using type = std::tuple<std::remove_reference_t<Args1>..., std::remove_reference_t<Args2>...>;
};
template <class T1, class T2>
using tuple_cat_rmr_t = typename tuple_cat_rmr<T1, T2>::type;

template <class T>
struct tuple_pop_type_front;
template <class T, class... Args>
struct tuple_pop_type_front<std::tuple<T, Args...>>
{
  using type = std::tuple<Args...>;
};
template <class T>
using tuple_pop_type_front_t = typename tuple_pop_type_front<T>::type;
template <class T, std::size_t N>
struct tuple_pop_type_front_n
  : tuple_pop_type_front_n<tuple_pop_type_front_t<T>, N - 1>
{};
template <class T>
struct tuple_pop_type_front_n<T, 0>
{
  using type = T;
};
template <class T, std::size_t N>
using tuple_pop_type_front_n_t = typename tuple_pop_type_front_n<T, N>::type;

template <class T, class Res = std::tuple<>>
struct tuple_pop_type_back;
template <class T, class... Args, class Res>
struct tuple_pop_type_back<std::tuple<T, Args...>, Res>
  : tuple_pop_type_back<std::tuple<Args...>, tuple_cat_t<Res, std::tuple<T>>>
{};
template <class T, class Res>
struct tuple_pop_type_back<std::tuple<T>, Res>
{
  using type = Res;
};
template <class T>
using tuple_pop_type_back_t = typename tuple_pop_type_back<T>::type;
template <class T, std::size_t N>
struct tuple_pop_type_back_n
  : tuple_pop_type_back_n<tuple_pop_type_back_t<T>, N - 1>
{};
template <class T>
struct tuple_pop_type_back_n<T, 0>
{
  using type = T;
};
template <class T, std::size_t N>
using tuple_pop_type_back_n_t = typename tuple_pop_type_back_n<T, N>::type;

template <std::size_t N = 1, class... Args, std::size_t... I>
auto tuple_pop_front_n_helper(std::tuple<Args...>& t, std::integer_sequence<std::size_t, I...>)
{
  return tuple_pop_type_front_n_t<std::tuple<Args...>, N>(std::get<I + N>(t)...);
}
template <std::size_t N = 1, class... Args>
auto tuple_pop_fron_n(std::tuple<Args...>& t)
{
  return tuple_pop_front_n_helper<N>(t, std::make_index_sequence<sizeof...(Args) - N>());
}
template <std::size_t N = 1, class T, std::size_t... I>
auto tuple_pop_back_n_helper(T& t, std::index_sequence<I...>)
{
  return tuple_pop_type_back_n_t<T, N>(std::get<I>(t)...);
}
template <std::size_t N = 1, class T>
auto tuple_pop_back_n(T& t)
{
  return tuple_pop_back_n_helper<N>(t, std::make_index_sequence<std::tuple_size_v<T> - N>());
}

template <class T>
concept iter_c = std::random_access_iterator<T>;

template <class T, std::size_t... I>
auto struct_to_tuple(std::index_sequence<I...>)
  -> std::tuple<decltype(boost::pfr::get<I>(T()))&...>;
template <class T>
using struct_to_tuple_t = decltype(struct_to_tuple<T>(std::make_index_sequence<boost::pfr::tuple_size_v<T>>()));

template <class T>
struct struct_to_tuple_type
{
  using type = struct_to_tuple_t<T>;
};
template <class... Args>
struct struct_to_tuple_type<std::tuple<Args...>>
{
  using type = std::tuple<Args...>;
};
template <class... Args>
struct struct_to_tuple_type<std::variant<Args...>>
{
  using type = std::variant<Args...>;
};
template <class T>
using struct_to_tuple_type_t = typename struct_to_tuple_type<T>::type;

template <class T>
struct gen_rule_type
{
  using type = T&;
};
template <class T>
struct gen_rule_type<T&&>
{
  using type = T;
};

template <class Lhs, class Rhs>
struct rule_binary_or_variant_cat
{
  using type = std::variant<Lhs, Rhs>;
};
template <class Lhs, class... RhsArgs>
struct rule_binary_or_variant_cat<Lhs, std::variant<RhsArgs...>>
{
  using type = std::variant<Lhs, RhsArgs...>;
};
template <class... LhsArgs, class Rhs>
struct rule_binary_or_variant_cat<std::variant<LhsArgs...>, Rhs>
{
  using type = std::variant<LhsArgs..., Rhs>;
};
template <class... LhsArgs, class... RhsArgs>
struct rule_binary_or_variant_cat<std::variant<LhsArgs...>, std::variant<RhsArgs...>>
{
  using type = std::variant<LhsArgs..., RhsArgs...>;
};
template <class Lhs, class Rhs>
using rule_binary_or_variant_cat_t = typename rule_binary_or_variant_cat<Lhs, Rhs>::type;
template <iter_c Iter, class Lhs, class Rhs>
struct rule_binary_or;
template <class T>
struct is_binary_or
{
  static constexpr bool value = false;
};
template <class... Args>
struct is_binary_or<rule_binary_or<Args...>>
{
  static constexpr bool value = true;
};
template <class T>
constexpr bool is_binary_or_v = is_binary_or<T>::value;

template <class Lhs, class Rhs>
struct rule_binary_expect_tuple_cat
{
  using type = std::tuple<Lhs, Rhs>;
};
template <>
struct rule_binary_expect_tuple_cat<void, void>
{
  using type = std::tuple<>;
};
template <class Lhs>
struct rule_binary_expect_tuple_cat<Lhs, void>
{
  using type = typename rule_binary_expect_tuple_cat<Lhs, std::tuple<>>::type;
};
template <class... LhsArgs>
struct rule_binary_expect_tuple_cat<std::tuple<LhsArgs...>, void>
{
  using type = typename rule_binary_expect_tuple_cat<std::tuple<LhsArgs...>, std::tuple<>>::type;
};
template <class Rhs>
struct rule_binary_expect_tuple_cat<void, Rhs>
{
  using type = typename rule_binary_expect_tuple_cat<std::tuple<>, Rhs>::type;
};
template <class Lhs, class... RhsArgs>
struct rule_binary_expect_tuple_cat<Lhs, std::tuple<RhsArgs...>>
{
  using type = std::tuple<Lhs, RhsArgs...>;
};
template <class... LhsArgs, class Rhs>
struct rule_binary_expect_tuple_cat<std::tuple<LhsArgs...>, Rhs>
{
  using type = std::tuple<LhsArgs..., Rhs>;
};
template <class... LhsArgs, class... RhsArgs>
struct rule_binary_expect_tuple_cat<std::tuple<LhsArgs...>, std::tuple<RhsArgs...>>
{
  using type = std::tuple<LhsArgs..., RhsArgs...>;
};
template <class Lhs, class Rhs>
using rule_binary_expect_tuple_cat_t = typename rule_binary_expect_tuple_cat<Lhs, Rhs>::type;

template <class Iter, class Lhs = void, class Rhs = void>
class rule_binary_expect;
template <class T>
struct is_binary_expect
{
  static constexpr bool value = false;
};
template <class... Args>
struct is_binary_expect<rule_binary_expect<Args...>>
{
  static constexpr bool value = true;
};
template <class T>
constexpr bool is_binary_expect_v = is_binary_expect<T>::value;

template <class Iter, class Lhs = void, class Rhs = void>
class rule_binary_right_shift;
template <class T>
struct is_binary_right_shift
{
  static constexpr bool value = false;
};
template <class... Args>
struct is_binary_right_shift<rule_binary_right_shift<Args...>>
{
  static constexpr bool value = true;
};
template <class T>
constexpr bool is_binary_right_shift_v = is_binary_right_shift<T>::value;

template <class T>
struct structure_tie
{
  constexpr static auto f(T& t)
  {
    return boost::pfr::structure_tie(t);
  }
};
template <class... Args>
struct structure_tie<std::tuple<Args...>>
{
  constexpr static auto& f(std::tuple<Args...>& t)
  {
    return t;
  }
};
template <class... Args>
struct structure_tie<std::variant<Args...>>
{
  constexpr static auto& f(std::variant<Args...>& t)
  {
    return t;
  }
};

template <class T>
struct remove_rreference
{
  using type = T;
};
template <class T>
struct remove_rreference<T&&>
{
  using type = T;
};
template <class T>
using remove_rreference_t = typename remove_rreference<T>::type;

template <class T>
struct remove_reference_from_tuple_types;
template <class... Args>
struct remove_reference_from_tuple_types<std::tuple<Args...>>
{
  using type = std::tuple<std::remove_reference_t<Args>...>;
};
template <class T>
using remove_reference_from_tuple_types_t = typename remove_reference_from_tuple_types<T>::type;


template <class... Args, std::size_t... I>
auto tuple_with_types_to_tuple_with_types_of_references_helper(std::tuple<Args...>& t, std::index_sequence<I...>)
{
  return std::tuple<Args&...>(std::get<I>(t)...);
}

template <class... Args>
auto tuple_with_types_to_tuple_with_types_of_references(std::tuple<Args...>& t)
{
  return tuple_with_types_to_tuple_with_types_of_references_helper(t, std::make_index_sequence<sizeof...(Args)>());
}
