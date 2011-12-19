#ifndef _MAKE_INDICES_H_
#define _MAKE_INDICES_H_

template <std::size_t...>
struct indices;

// Define adding the Nth index...
// Notice one argument, Type, is discarded each recursive call.
// Notice N is added to the end of Indices... .
// Notice N is incremented with the recursion.
template <std::size_t N, typename Indices, typename... Types>
struct make_indices_impl;

template <
  std::size_t N,
  std::size_t... Indices,
  typename Type,
  typename... Types
>
struct make_indices_impl<N, indices<Indices...>, Type, Types...>
{
  typedef
    typename make_indices_impl<
      N+1,
      indices<Indices...,N>,
      Types...
    >::type
    type
  ;
};

// Define adding the last index...
// Notice no Type or Types... are left.
// Notice the full solution is emitted into the container.
template <std::size_t N, std::size_t... Indices>
struct make_indices_impl<N, indices<Indices...>>
{
  typedef indices<Indices...> type;
};

// Compute the indices starting from zero...
// Notice indices container is empty.
// Notice Types... will be all of the tuple element types.
// Notice this refers to the full solution (i.e., via ::type).
template <std::size_t N, typename... Types>
struct make_indices
{
  typedef
    typename make_indices_impl<0, indices<>, Types...>::type
    type
  ;
};

#endif /* _MAKE_INDICES_H_ */
