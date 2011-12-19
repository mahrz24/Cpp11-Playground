#ifndef _APPLY_H_
#define _APPLY_H_

#include "make_indices.hpp"

#include <tuple>

template <typename Op, typename... Args>
auto apply(Op&& op, Args&&... args)
  -> typename std::result_of<Op(Args...)>::type
{
  return op( std::forward<Args>(args)... );
}

template <typename Indices>
struct apply_tuple_impl;

template <
  typename Op,
  typename... OpArgs,
  typename Indices = typename make_indices<0, OpArgs...>::type,
  template <typename...> class T = std::tuple
>
auto apply_tuple(Op&& op, T<OpArgs...>&& t)
  -> typename std::result_of<Op(OpArgs...)>::type
{
  return apply_tuple_impl<Indices>::apply_tuple(
    std::forward<Op>(op),
    std::forward<T<OpArgs...>>(t)
  );
}

template <
  typename Op,
  typename... OpArgs,
  typename Indices = typename make_indices<0, OpArgs...>::type,
  template <typename...> class T = std::tuple
>
auto apply_tuple(Op&& op, T<OpArgs...> const& t)
  -> typename std::result_of<Op(OpArgs...)>::type
{
  return apply_tuple_impl<Indices>::apply_tuple(
    std::forward<Op>(op),
    std::forward<T<OpArgs...> const>(t)
  );
}

template <
  template <std::size_t...> class I,
  std::size_t... Indices
>
struct apply_tuple_impl<I<Indices...>>
{
  template <
    typename Op,
    typename... OpArgs,
    template <typename...> class T = std::tuple
  >

  static auto apply_tuple(Op&& op, T<OpArgs...> && t)
    -> typename std::result_of<Op(OpArgs...)>::type
  {
    return op( std::forward<OpArgs>(std::get<Indices>(t))... );
  }

  template <
    typename Op,
    typename... OpArgs,
    template <typename...> class T = std::tuple
  >
  static auto apply_tuple(Op&& op, T<OpArgs...> const& t)
    -> typename std::result_of<Op(OpArgs...)>::type
  {
    return op( std::forward<OpArgs const>(std::get<Indices>(t))... );
  }
};

#endif /* _APPLY_H_ */
