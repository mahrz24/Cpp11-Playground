#ifndef _TEST_H_
#define _TEST_H_


#include <type_traits>
#include <iostream>

#include "apply.hpp"

template<typename ... T>
struct always_false
{
  static const bool value = false;
};

template<typename T, 
         typename ...>
struct find_type_impl;

template<typename T>
struct find_type_impl<T,T>
{
  static T&& find_type(T&& head)
  {
    return std::forward<T>(head);
  }

  static T const& find_type_(T const& head)
  {
    return head;
  }
};

template<typename T, typename U>
struct find_type_impl<T,U>
{
  static T&& find_type(U&& head)
  {
    static_assert(std::is_same<T,U>::value, "Type not in type list");
  }

  static T const& find_type_(U const& head)
  {
    static_assert(std::is_same<T,U>::value, "Type not in type list");
  }
};

template<typename T, typename ... Tail>
struct find_type_impl<T,T,Tail...>
{
  static T&& find_type(T&& head, Tail&& ... tail)
  {
    return std::forward<T>(head);
  }

  static T const& find_type_(T const& head, Tail const& ... tail)
  {
    return head;
  }
};

template<typename T, typename Head, typename ... Tail>
struct find_type_impl<T,Head, Tail...>
{
  static T&& find_type(Head&& head, Tail&& ... tail)
  {
    return find_type_impl<T,Tail...>::find_type(std::forward<Tail>(tail)...);
  }

  static T const& find_type_(Head const& head, Tail const& ... tail)
  {
    return find_type_impl<T,Tail...>::find_type_(std::forward<Tail const>(tail)...);
  }
};

template <typename T, 
          typename Head, 
          typename ... Tail,
          template <typename...> class U = std::tuple>
T&& find_value_by_type(U<Head, Tail...>&& t)
{
  return apply_tuple(find_type_impl<T, Head, Tail...>::find_type, 
                     std::forward<U<Head, Tail...>>(t));
}

template <typename T, 
          typename Head, 
          typename ... Tail,
          template <typename...> class U = std::tuple>
T const& find_value_by_type(U<Head, Tail...> const& t)
{
  return apply_tuple(find_type_impl<T, Head, Tail...>::find_type_, 
                     std::forward<U<Head, Tail...> const>(t));
}

// This test is about set functions 

////////////////////////////////////////////////////////////
// Type Set
////////////////////////////////////////////////////////////

template<typename ... Types>
struct type_set;

typedef type_set<> empty_set;

////////////////////////////////////////////////////////////
// Is Element
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct is_element;

template<typename A>
struct is_element<A, empty_set>
{
  static const  bool value = false;
};

template<typename A, typename HeadB, typename ... TypesB>
struct is_element<A, type_set<HeadB, TypesB...>>
{
  static const bool value = 
    std::is_same<A,HeadB>::value ||
    is_element<A, type_set<TypesB...>>::value;
};

template<typename A, typename B>
struct is_element
{
  static const bool value = false;

  static_assert(always_false<A,B>::value, 
                "Second template argument is not a type set");  
};

////////////////////////////////////////////////////////////
// Is Subset
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct is_subset;

template<typename ... TypesB>
struct is_subset<empty_set, type_set<TypesB...>>
{
  static const bool value = true;
};

template<typename HeadA,
         typename ... TypesA,
         typename ... TypesB>
struct is_subset<type_set<HeadA, TypesA...>, type_set<TypesB...>>
{
  static const bool value = 
    is_element<HeadA, type_set<TypesB...>>::value &&
    is_subset<type_set<TypesA...>, type_set<TypesB...>>::value;
};

template<typename A, typename B>
struct is_subset
{
  static const bool value = false;

  static_assert(always_false<A,B>::value, 
                "Template arguments are no type sets");  
};

////////////////////////////////////////////////////////////
// Set Equality 
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct is_set_equal
{
  static const bool value = is_subset<A,B>::value && is_subset<B,A>::value;
};

////////////////////////////////////////////////////////////
// Conditional Type Set Construction
////////////////////////////////////////////////////////////

template<bool condition,
         typename NewA,
         typename A>
struct cons_set_if;

template<typename NewA,
         typename ... TypesA>
struct cons_set_if<true, NewA, type_set<TypesA...>>
{
  typedef type_set<NewA, TypesA...> type;
};

template<typename NewA,
         typename ... TypesA>
struct cons_set_if<false, NewA, type_set<TypesA...>>
{
  typedef type_set<TypesA...> type;
};

template<bool condition,
         typename NewA,
         typename A>
struct cons_set_if
{
  typedef void type;
  static_assert(always_false<NewA,A>::value, 
                "Template arguments are no type sets");  
};

////////////////////////////////////////////////////////////
// Set Reduction 
////////////////////////////////////////////////////////////

template<typename A>
struct reduce_set;

template<>
struct reduce_set<empty_set>
{
  typedef empty_set type;
};

template<typename HeadA, typename ... TypesA>
struct reduce_set<type_set<HeadA, TypesA...>>
{
  typedef typename reduce_set<type_set<TypesA...>>::type tail_reduced;
  typedef typename cons_set_if<!is_element<HeadA, tail_reduced>::value, 
                               HeadA, 
                               tail_reduced>::type
    type;
};

template<typename A>
struct reduce_set
{
  typedef void type;
  static_assert(always_false<A>::value, 
                "Template arguments are no type sets");  
};

////////////////////////////////////////////////////////////
// Set Union 
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct set_union;

template<typename ... TypesA, typename ... TypesB>
struct set_union<type_set<TypesA...>, type_set<TypesB...>>
{
  typedef typename reduce_set<type_set<TypesA..., TypesB...>>::type type;
};

template<typename A, typename B>
struct set_union
{
  typedef void type;
  static_assert(always_false<A,B>::value, 
                "Template arguments are no type sets");  
};

////////////////////////////////////////////////////////////
// Set Intersection 
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct set_intersection;

template<typename ... TypesB>
struct set_intersection<empty_set,type_set<TypesB...>>
{
  typedef empty_set type;
};

template<typename HeadA, typename ... TypesA, typename ... TypesB>
struct set_intersection<type_set<HeadA, TypesA...>, type_set<TypesB...>>
{
  typedef typename set_intersection<type_set<TypesA...>, 
                                    type_set<TypesB...>>::type tail_intersection;
  typedef typename reduce_set<
    typename cons_set_if<is_element<HeadA, 
                           type_set<TypesB...>>::value, 
                HeadA, 
                tail_intersection>::type
    >::type type;
};

template<typename A, typename B>
struct set_intersection
{
  typedef void type;
  static_assert(always_false<A,B>::value, 
                "Template arguments are no type sets");  
};

////////////////////////////////////////////////////////////
// Set Union 
////////////////////////////////////////////////////////////

template<typename A, typename B>
struct set_cross;

template<typename ... Types>
struct set_cross<empty_set, type_set<Types...>>
{
  typedef empty_set type;
};

template<typename ... Types>
struct set_cross<type_set<Types...>, empty_set>
{
  typedef empty_set type;
};

template<typename HeadA,
         typename HeadB>
struct set_cross<type_set<HeadA>, type_set<HeadB>>
{
  typedef type_set<std::tuple<HeadA, HeadB>> type;
};

template<typename HeadA, typename ... TypesA, 
         typename HeadB, typename ... TypesB>
struct set_cross<type_set<HeadA, TypesA...>, type_set<HeadB, TypesB...>>
{
private:
  typedef typename set_cross<type_set<HeadA>, type_set<HeadB>>::type head_square;
  typedef typename set_cross<type_set<HeadA>, type_set<TypesB...>>::type head_vert;
  typedef typename set_cross<type_set<TypesA...>, type_set<HeadB>>::type head_horiz;
  typedef typename set_cross<type_set<TypesA...>, type_set<TypesB...>>::type tail;
  typedef typename set_union<head_square, head_vert>::type u1;
  typedef typename set_union<u1, head_horiz>::type u2;
public:
  typedef typename set_union<u2, tail>::type type;
};

template<typename A, typename B>
struct set_cross
{
  typedef void type;
  static_assert(always_false<A,B>::value, 
                "Template arguments are no type sets");  
};


#endif /* _TEST_H_ */

















