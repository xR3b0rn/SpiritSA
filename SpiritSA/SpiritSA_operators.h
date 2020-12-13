#pragma once

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>&>(std::move(lhs.child()), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_expect<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator|(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator|(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator|(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator|(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_or<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), rhs.child());
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator-(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_minus<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator-(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_minus<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator-(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_minus<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator-(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_minus<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), rhs.child());
}

template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_right_shift<Iter, LhsC<Iter, LhsArgs...>&, RhsC<Iter, RhsArgs...>&>(lhs.child(), rhs.child());
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_right_shift<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>>(rule_base<LhsC<Iter, LhsArgs...>>& lhs, rule_base<RhsC<Iter, RhsArgs...>>&& rhs)
{
  return rule_binary_right_shift<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(lhs.child(), std::move(rhs.child()));
}
template <
    class Iter
  , template <class...> class LhsC, class... LhsArgs
  , template <class...> class RhsC, class... RhsArgs>
constexpr auto operator>>(rule_base<LhsC<Iter, LhsArgs...>>&& lhs, rule_base<RhsC<Iter, RhsArgs...>>& rhs)
{
  return rule_binary_right_shift<Iter, LhsC<Iter, LhsArgs...>, RhsC<Iter, RhsArgs...>>(std::move(lhs.child()), rhs.child());
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
  return rule_unary_star<Iter, R<Iter, RArgs...>>(std::move(r.child()));
}

template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator+(rule_base<R<Iter, RArgs...>>& r)
{
  return rule_unary_plus<Iter, R<Iter, RArgs...>&>(r.child());
}
template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator+(rule_base<R<Iter, RArgs...>>&& r)
{
  return rule_unary_plus<Iter, R<Iter, RArgs...>>(std::move(r.child()));
}

template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator!(rule_base<R<Iter, RArgs...>>& r)
{
  return rule_unary_not<Iter, R<Iter, RArgs...>&>(r.child());
}
template <
    class Iter
  , template <class...> class R, class... RArgs>
constexpr auto operator!(rule_base<R<Iter, RArgs...>>&& r)
{
  return rule_unary_not<Iter, R<Iter, RArgs...>>(std::move(r.child()));
}
