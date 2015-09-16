#ifndef FUNCTION_TRAITS_H_
#define FUNCTION_TRAITS_H_

#include <functional>

template<typename T>
struct function_traits;

template<typename Ret,typename ... Args>
struct function_traits<Ret(Args...)>
{
	using return_type = Ret;
	using function_type = Ret(Args...);
	using pointer = Ret(*)(Args...);
	using stl_function_type = std::function<function_type>; 
    enum { arity = sizeof...(Args) };
	template< size_t I >
	struct args{
		using type = typename std::tuple_element<I,std::tuple<Args...>>::type;
	};
};

template<typename Ret,typename ... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>{
};


template<typename Ret,typename ... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>{
};

#define FUNCTION_TRAITS(...) \
template<typename ClassType,typename Ret,typename ... Args> \
struct function_traits<Ret(ClassType::*)(Args...) __VA_ARGS__> : function_traits<Ret(Args...)>{ \
}; \

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>{
};

template<typename FuncType>
typename function_traits<FuncType>::stl_function_type to_function(FuncType&& func){
	return static_cast<typename function_traits<FuncType>::stl_function_type>(func);
};

#endif
