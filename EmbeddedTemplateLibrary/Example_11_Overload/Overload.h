// Overload.h

#ifndef OVERLOAD_H
#define OVERLOAD_H

#include "etl/variant.h"
#include "etl/vector.h"
#include "etl/overload.h"

// From overloaded
// This is declaring a set of lambdas to be overloaded.
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; // Not available
// This is needed for the cppreference example although NOT in that text!
// I think this will not work in C++11. I need an alternative
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// This will need extending as the variadic version does not work.
template<class T1, class T2> struct overloaded : T1, T2 
{ 
	using T1::operator();
	using T2::operator();
};

// Helper function no longer variadic

template<class T1, class T2>
auto makeOverload(T1 t1, T2 t2) -> overloaded<T1, T2> {
  return overloaded<T1, T2>{t1, t2};	
}
/*
template<class... Ts>
auto makeOverload(Ts... ts) -> overloaded<Ts...> {
  return overloaded<Ts...>{ts...};	
}
*/

template <class T>
struct identity {
	using type = T;
};

template <class T>
using identity_t = typename identity<T>::type;

// I am not sure if this works in C++11.
template <template <class T> class Base = identity_t, class... Args>
struct SelfReturning {
	using RetType = Base<SelfReturning>;
	using FuncType = RetType(*)(Args... args);
	
	SelfReturning(FuncType fn) : func{ fn } {}
	RetType operator () (Args... args) const {
		return func(etl::forward<Args>(args)...);
	}
	
	FuncType func;
	
	template <class... AltArgs>
	using WithArgs = SelfReturning<Base, AltArgs...>;
};

#endif