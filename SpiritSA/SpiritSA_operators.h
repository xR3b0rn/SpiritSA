
#pragma once


#define DEFINE_UNARY_OPERATOR(OP, CL)           \
  template <class R>                            \
    requires                                    \
      std::is_convertible_v<                    \
          std::decay_t<R>&                      \
         , rule_base<void>&>                    \
  constexpr auto operator OP (R&& r)            \
  {                                             \
    using iter_t =                              \
        typename std::decay_t<R>::iter_t;       \
    return CL <                                 \
        iter_t                                  \
      , remove_rreference_t<R>>(                \
          std::forward<R>(r));                  \
  }

#define DEFINE_BINARY_OPERATOR(OP, CL)          \
  template <class Lhs, class Rhs>               \
    requires                                    \
      std::is_convertible_v<                    \
          std::decay_t<Lhs>&                    \
         , rule_base<void>&> &&                 \
      std::is_convertible_v<                    \
          std::decay_t<Rhs>&                    \
         , rule_base<void>&>                    \
  auto operator OP (Lhs&& lhs, Rhs&& rhs)       \
  {                                             \
    static_assert(                              \
        std::is_same_v<                         \
            std::decay_t<Lhs>::iter_t           \
          , std::decay_t<Rhs>::iter_t>);        \
    using iter_t =                              \
        typename std::decay_t<Lhs>::iter_t;     \
    return CL <                                 \
        iter_t                                  \
      , remove_rreference_t<Lhs>                \
      , remove_rreference_t<Rhs>>(              \
          std::forward<Lhs>(lhs)                \
        , std::forward<Rhs>(rhs));              \
  }

DEFINE_UNARY_OPERATOR(!, rule_unary_not);
DEFINE_UNARY_OPERATOR(-, rule_unary_minus);
DEFINE_UNARY_OPERATOR(+, rule_unary_plus);
DEFINE_UNARY_OPERATOR(*, rule_unary_star);

DEFINE_BINARY_OPERATOR(>>, rule_binary_right_shift);
DEFINE_BINARY_OPERATOR(>, rule_binary_expect);
DEFINE_BINARY_OPERATOR(|, rule_binary_or);
DEFINE_BINARY_OPERATOR(-, rule_binary_minus);
