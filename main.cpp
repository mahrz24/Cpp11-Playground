#include "test.hpp"

#include <iostream>
#include <functional>

using namespace std;

#define RANDOM_VAR(x) struct x { x() {}; x(int a) : a_(a) {} ; int a_;};

RANDOM_VAR(A)
RANDOM_VAR(B)
RANDOM_VAR(C)

int add(int  a, int  b)
{
  return a+b;
}

int main(int argc, char *argv[])
{
  tuple<int,bool,string> myTuple(1, false, "Hello world");
  
  string test = find_value_by_type<string>(myTuple);
  cout << test << endl;

  int test2 =  find_value_by_type<int>(make_tuple(4,5,false,"Foo"));
  cout << test2 << endl;

  cout << is_subset<type_set<int,string>, type_set<int,string,int>>::value << endl;
  cout << is_subset<type_set<int,string>, type_set<int>>::value << endl;

  cout << is_set_equal<type_set<int, string>, reduce_set<type_set<int, string, int>>::type>::value << endl;
  cout << is_set_equal<set_union<type_set<int, string>,
                                 type_set<int, bool, double>>::type, 
                       type_set<int, string, bool, double>>::value << endl;

  cout << is_set_equal<set_intersection<type_set<int, string>,
                                 type_set<int, bool, double>>::type, 
                       type_set<int>>::value << endl;

  cout << 
    is_set_equal<
      set_cross<
        type_set<int, string>,
        type_set<int, bool>
        >::type, 
      type_set<
        tuple<int,int>,
        tuple<int,bool>,
        tuple<string,int>, 
        tuple<string,bool>
       >
      >::value << endl;

  return 0;
}







