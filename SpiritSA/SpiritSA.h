
#pragma once

#include <iostream>
#include <variant>
#include <type_traits>
#include <optional>
#include <sstream>
#include <tuple>
#include <iterator>
#include <charconv>

#include <boost/pfr.hpp>

template <class T>
class recursive_wrapper
{
public:
  recursive_wrapper()
    : _data(std::make_unique<T>())
  {}
  recursive_wrapper(const recursive_wrapper<T>& other)
  {
    _data = std::make_unique<T>(*other._data);
  }
  recursive_wrapper& operator=(const recursive_wrapper<T>& other)
  {
    _data = std::make_unique<T>(*other._data);
  }
  recursive_wrapper(recursive_wrapper<T>&& other)
  {
    _data = std::move(other._data);
  }
  recursive_wrapper& operator=(recursive_wrapper<T>&& other)
  {
    _data = std::move(other._data);
    return this;
  }
  recursive_wrapper(const T& data)
  {
    _data = std::make_unique<T>(data);
  }
  recursive_wrapper<T>& operator=(const T& data)
  {
    _data = std::make_unique<T>(data);
    return *this;
  }
  recursive_wrapper(T&& data)
  {
    _data = std::make_unique<T>(std::move(data));
  }
  recursive_wrapper<T>& operator=(const T&& data)
  {
    _data = std::make_unique<T>(std::move(data));
    return *this;
  }
  operator T&()
  {
    return *_data;
  }
  operator const T&()
  {
    return *_data;
  }

private:
  std::unique_ptr<T> _data;
};

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

template <class Child>
class rule_base
{
public:
  using child_t = Child;
  child_t& child()
  {
    return static_cast<child_t&>(*this);
  }
};

template <iter_c Iter, class Res>
class rule
  : public rule_base<rule<Iter, Res>>
{
public:
  using unfold_t = struct_to_tuple_type_t<Res>&;
  using fold_t = Res;
  static constexpr std::size_t N = tuple_size_v<std::remove_reference_t<fold_t>>;

  template <class R>
  constexpr void operator%=(const R& r)
  {
    std::string name;
    name = typeid(fold_t).name();
    _f =
      [this, r](Iter begin, Iter end, unfold_t res)
      {
        return r.parse(begin, end, res);
      };
  }
  template <class Res2>
  constexpr bool parse(Iter begin, Iter end, Res2& res) const
  {
    auto tmp_res = structure_tie<Res>::f(res);
    if (_f(begin, end, tmp_res))
    {
      if (_on_match)
      {
        _on_match(res);
      }
      return true;
    }
    return false;
  }

private:
  std::function<bool(Iter begin, Iter end, unfold_t)> _f;
  std::function<void(Res& m)> _on_match;
};

template <class Iter, class Lhs, class Rhs>
class rule_binary_expect
  : public rule_base<rule_binary_expect<Iter, Lhs, Rhs>>
{
public:
  using Lhs_t = std::remove_reference_t<Lhs>;
  using Rhs_t = std::remove_reference_t<Rhs>;
  using fold_t = rule_binary_expect_tuple_cat_t<typename Lhs_t::fold_t, typename Rhs_t::fold_t>;
  static constexpr std::size_t N = Lhs_t::N + Rhs_t::N;

  constexpr rule_binary_expect(Lhs lhs, Rhs rhs)
    : _lhs(lhs)
    , _rhs(rhs)
  {}
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res res) const
  {
    auto old = begin;
    if constexpr (is_binary_expect_v<Lhs_t>)
    {
      if (!_lhs.parse(begin, end, tuple_pop_back_n<Rhs_t::N>(res)))
      {
        begin = old;
        return false;
      }
    }
    else if constexpr (std::is_same_v<Lhs_t::fold_t, void>)
    {
      if (!_lhs.parse(begin, end))
      {
        begin = old;
        return false;
      }
    }
    else
    {
      if (!_lhs.parse(begin, end, get<0>(res)))
      {
        begin = old;
        return false;
      }
    }
    if constexpr (is_binary_expect_v<Rhs_t>)
    {
      if (!_rhs.parse(begin, end, tuple_pop_front_n<Lhs_t::N>(res)))
      {
        begin = old;
        throw std::runtime_error("expected token");
      }
    }
    else if constexpr (std::is_same_v<Rhs_t::fold_t, void>)
    {
      if (!_rhs.parse(begin, end))
      {
        begin = old;
        throw std::runtime_error("expected token");
      }
    }
    else
    {
      if (!_rhs.parse(begin, end, get<Lhs_t::N>(res)))
      {
        begin = old;
        throw std::runtime_error("expected token");
      }
    }
    return true;
  }

private:
  Lhs _lhs;
  Rhs _rhs;
};
template <iter_c Iter, class R, class T2 = void>
class rule_unary_star
  : public rule_base<rule_unary_star<Iter, R, T2>>
{
public:
  using R_t = std::remove_reference_t<R>;
  using fold_t = std::vector<std::remove_reference_t<typename R_t::fold_t>>&;
  static constexpr std::size_t N = 1;

  constexpr rule_unary_star(R r)
    : _r(r)
  {}
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    std::remove_reference_t<typename R_t::fold_t> t;
    while (_r.parse(begin, end, t))
    {
      res.push_back(t);
    }
    return true;
  }

private:
  R _r;
};

template <iter_c Iter, class R, class T2 = void>
class rule_unary_minus
  : public rule_base<rule_unary_minus<Iter, R, T2>>
{
public:
  using R_t = std::remove_reference_t<R>;
  using fold_t = std::optional<std::remove_reference_t<typename R_t::fold_t>>&;
  static constexpr std::size_t N = 1;

  constexpr rule_unary_minus(R r)
    : _r(r)
  {}
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    std::remove_reference_t<typename R_t::fold_t> t;
    if (_r.parse(begin, end, t))
    {
      res = std::move(t);
    }
    return true;
  }

private:
  R _r;
};

template <iter_c Iter, class Lhs, class Rhs>
class rule_binary_or<Iter, Lhs, Rhs>
  : public rule_base<rule_binary_or<Iter, Lhs, Rhs>>
{
public:
  using Lhs_t = std::remove_reference_t<Lhs>;
  using Rhs_t = std::remove_reference_t<Rhs>;
  using fold_t = rule_binary_or_variant_cat_t<std::remove_reference_t<typename Lhs_t::fold_t>, std::remove_reference_t<typename Rhs_t::fold_t>>&;
  static constexpr std::size_t N = 1;
  
  constexpr rule_binary_or(Lhs lhs, Rhs rhs)
    : _lhs(lhs)
    , _rhs(rhs)
  {}

  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res res) const
  {
    if constexpr (is_binary_or_v<Lhs_t>)
    {
      if (_lhs.parse(begin, end, res))
      {
        return true;
      }
    }
    else if constexpr (std::is_same_v<typename Lhs_t::fold_t, void>)
    {
      if (_lhs.parse(begin, end))
      {
        return true;
      }
    }
    else
    {
      std::remove_reference_t<typename Lhs_t::fold_t> tmp;
      if (_lhs.parse(begin, end, tmp))
      {
        res = std::move(tmp);
        return true;
      }
    }
    if constexpr (is_binary_or_v<Rhs_t>)
    {
      if (_rhs.parse(begin, end, res))
      {
        return true;
      }
    }
    else if constexpr (std::is_same_v<typename Rhs_t::fold_t, void>)
    {
      if (_rhs.parse(begin, end))
      {
        return true;
      }
    }
    else
    {
      std::remove_reference_t<typename Rhs_t::fold_t> tmp;
      if (_rhs.parse(begin, end, tmp))
      {
        res = std::move(tmp);
        return true;
      }
    }
    return false;
  }

private:
  Lhs _lhs;
  Rhs _rhs;
};

template <class Iter, class T, class T1 = void>
class rule_number
  : public rule_base<rule_number<Iter, T, T1>>
{
public:
  using fold_t = T&;
  static constexpr std::size_t N = 1;
  
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    std::istringstream iss(begin);
    if (begin == end)
    {
      return false;
    }
    iss >> res;
    if (iss.eof())
    {
      begin = end;
      return true;
    }
    if (iss.tellg() > 0)
    {
      begin += iss.tellg();
      return true;
    }
    return false;
  }
};
rule_number<const char*, int64_t> int_;
rule_number<const char*, double> double_;

template <class Iter, class T1 = void, class T2 = void>
class rule_lit
  : public rule_base<rule_lit<Iter, T1, T2>>
{
public:
  using fold_t = void;
  static constexpr std::size_t N = 0;
  
  constexpr rule_lit(const std::string& str)
    : _str(str)
  {}
  constexpr bool parse(Iter& begin, Iter end) const
  {
    if (std::strncmp(begin + 1, _str.c_str(), _str.size()) == 0)
    {
      begin += _str.size();
      return true;
    }
    return false;
  }

private:
  std::string _str;
};
using lit = rule_lit<const char*>;




template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>&>(std::move(lhs.child()), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator|(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator|(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator|(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
auto operator|(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), rhs.child());
}

template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator-(rule_base<R<Iter, RArgs...>>& r)
{
  return rule_unary_minus<Iter, R<Iter, RArgs...>&>(r.child());
}
template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator-(rule_base<R<Iter, RArgs...>>&& r)
{
  return rule_unary_minus<Iter, R<Iter, RArgs...>&>(std::move(r.child()));
}
  
template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator*(rule_base<R<Iter, RArgs...>>& r)
{
  return rule_unary_star<Iter, R<Iter, RArgs...>&>(r.child());
}
template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator*(rule_base<R<Iter, RArgs...>>&& r)
{
  return rule_unary_star<Iter, R<Iter, RArgs...>&>(std::move(r.child()));
}
