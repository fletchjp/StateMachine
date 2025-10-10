// StateMachine.hpp

// This is a C++17 file from the work of Tamir Bahar
// https://tamir.dev/posts/a-functional-style-state-macnine-in-cpp
// https://tamir.dev/posts/a-functional-style-state-macnine-in-cpp-part2
// cppreference has this as C++26.
// No that is extension as member function.

// Header being developed from StateMachine.cxx

#pragma once

#include <variant>
#include <vector>

// From overloaded
// This is declaring a set of lambdas to be overloaded.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// This is needed for the cppreference example although NOT in that text!
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <class T>
struct identity {
	using type = T;
};

template <class T>
using identity_t = typename identity<T>::type;

template <template <class T> class Base = identity_t, class... Args>
struct SelfReturning {
	using RetType = Base<SelfReturning>;
	using FuncType = RetType(*)(Args... args);
	
	SelfReturning(FuncType fn) : func{ fn } {}
	RetType operator () (Args... args) const {
		return func(std::forward<Args>(args)...);
	}
	
	FuncType func;
	
	template <class... AltArgs>
	using WithArgs = SelfReturning<Base, AltArgs...>;
};

