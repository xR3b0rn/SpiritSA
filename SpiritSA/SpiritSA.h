
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

#include "SpiritSA_helper.h"

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
  operator const T&() const
  {
    return *_data;
  }

private:
  std::unique_ptr<T> _data;
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
    _f =
      [this, r](Iter& begin, Iter end, unfold_t res)
      {
        return r.parse(begin, end, res);
      };
  }
  template <class Res2>
  constexpr bool parse(Iter& begin, Iter end, Res2& res) const
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
  std::function<bool(Iter& begin, Iter end, unfold_t)> _f;
  std::function<void(Res& m)> _on_match;
};

template <iter_c Iter>
class rule<Iter, std::string>
  : public rule_base<rule<Iter, std::string>>
{
public:
  using fold_t = std::string&;
  static constexpr std::size_t N = 1;

  template <class R>
  constexpr void operator%=(const R& r)
  {
    _f =
      [this, r](Iter& begin, Iter end, std::string& res)
      {
        return r.parse(begin, end, res);
      };
  }
  template <class Res2>
  constexpr bool parse(Iter& begin, Iter end, Res2& res) const
  {
    if (_f(begin, end, res))
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
  std::function<bool(Iter& begin, Iter end, std::string&)> _f;
  std::function<void(std::string& m)> _on_match;
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
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    auto old = begin;
    if constexpr (is_binary_expect_v<Lhs_t> || is_binary_right_shift_v<Lhs_t>)
    {
      auto t = tuple_pop_back_n<Rhs_t::N>(res);
      if (!_lhs.parse(begin, end, t))
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
    if constexpr (is_binary_expect_v<Rhs_t> || is_binary_right_shift_v<Rhs_t>)
    {
      if (!_rhs.parse(begin, end, tuple_pop_front_n<Lhs_t::N>(res)))
      {
        begin = old;
        throw std::runtime_error("expected token");
      }
    }
    else if constexpr (std::is_same_v<Rhs_t::fold_t, void>)
    {
      if (!_rhs.parse(begin, end, &res))
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

template <class Iter, class Lhs, class Rhs>
class rule_binary_right_shift
  : public rule_base<rule_binary_right_shift<Iter, Lhs, Rhs>>
{
public:
  using Lhs_t = std::remove_reference_t<Lhs>;
  using Rhs_t = std::remove_reference_t<Rhs>;
  using fold_t = rule_binary_expect_tuple_cat_t<typename Lhs_t::fold_t, typename Rhs_t::fold_t>;
  static constexpr std::size_t N = Lhs_t::N + Rhs_t::N;

  constexpr rule_binary_right_shift(Lhs lhs, Rhs rhs)
    : _lhs(lhs)
    , _rhs(rhs)
  {}
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    auto old = begin;
    if constexpr (is_binary_expect_v<Lhs_t> || is_binary_right_shift_v<Lhs_t>)
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
    if constexpr (is_binary_expect_v<Rhs_t> || is_binary_right_shift_v<Rhs_t>)
    {
      if (!_rhs.parse(begin, end, tuple_pop_front_n<Lhs_t::N>(res)))
      {
        begin = old;
        return false;
      }
    }
    else if constexpr (std::is_same_v<Rhs_t::fold_t, void>)
    {
      if (!_rhs.parse(begin, end))
      {
        begin = old;
        return false;
      }
    }
    else
    {
      if (!_rhs.parse(begin, end, get<Lhs_t::N>(res)))
      {
        begin = old;
        return false;
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
    auto& tr = t;
    while (_r.parse(begin, end, tr))
    {
      res.push_back(t);
    }
    return true;
  }

private:
  R _r;
};
template <iter_c Iter, class R, class T2 = void>
class rule_unary_plus
  : public rule_base<rule_unary_plus<Iter, R, T2>>
{
public:
  using R_t = std::remove_reference_t<R>;
  using fold_t = std::vector<std::remove_reference_t<typename R_t::fold_t>>&;
  static constexpr std::size_t N = 1;

  constexpr rule_unary_plus(R r)
    : _r(r)
  {}
  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    std::remove_reference_t<typename R_t::fold_t> t;
    auto& tr = t;
    while (_r.parse(begin, end, tr))
    {
      res.push_back(t);
    }
    return res.size();
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

template <iter_c Iter, class R, class T2 = void>
class rule_unary_not
  : public rule_base<rule_unary_not<Iter, R, T2>>
{
public:
  using R_t = std::remove_reference_t<R>;
  using fold_t = void;
  static constexpr std::size_t N = 0;

  constexpr rule_unary_not(R r)
    : _r(r)
  {}
  constexpr bool parse(Iter& begin, Iter end) const
  {
    return !_r.parse(begin, end);
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
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
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

template <iter_c Iter, class Lhs, class Rhs>
class rule_binary_minus
  : public rule_base<rule_binary_minus<Iter, Lhs, Rhs>>
{
public:
  using Lhs_t = std::remove_reference_t<Lhs>;
  using Rhs_t = std::remove_reference_t<Rhs>;
  using fold_t = typename Lhs_t::fold_t&;
  static constexpr std::size_t N = 1;
  
  constexpr rule_binary_minus(Lhs lhs, Rhs rhs)
    : _lhs(lhs)
    , _rhs(rhs)
  {}

  template <class Res>
  constexpr bool parse(Iter& begin, Iter end, Res& res) const
  {
    auto old = begin;
    if (_rhs.parse(begin, end, &res))
    {
      begin = old;
      return false;
    }
    if (!_lhs.parse(begin, end, res))
    {
      begin = old;
      return false;
    }
    return true;
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
rule_number<const char*, char> char_;

template <class F>
struct ParamT;

template <class Iter, class T = void, class T2 = void>
class rule_lit
  : public rule_base<rule_lit<Iter, T, T2>>
{
public:
  using fold_t = void;
  static constexpr std::size_t N = 0;
  
  constexpr rule_lit(std::string (*str)(T) )
    : _str(str)
  {}
  constexpr rule_lit(const std::string& str)
    : _str([str]() { return str; })
  {}
  constexpr bool parse(Iter& begin, Iter end, T* t = nullptr) const
  {
    auto old = begin;
    if constexpr (std::is_same_v<void, T>)
    {
      auto str = _str();
      if (std::strncmp(begin, str.c_str(), str.size()) == 0)
      {
        begin += str.size();
        return true;
      }
    }
    else
    {
      if (std::strncmp(begin, _str(*t).c_str(), _str(*t).size()) == 0)
      {
        begin += _str(*t).size();
        return true;
      }
    }
    begin = old;
    return false;
  }

private:
  std::function<std::string(T)> _str;
};
template <class T = void>
using lit = rule_lit<const char*, T>;

#include "SpiritSA_operators.h"
