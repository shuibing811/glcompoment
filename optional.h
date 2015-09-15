#ifndef OPTIONAL_H_
#define OPTIONAL_H_

#include <memory>
#include <type_traits>

namespace gl {
	class bad_optional_access : public std::logic_error
	{
	public:
		bad_optional_access()
			: std::logic_error("Attempted to access the value of an uninitialized optional object.")
		{}
	};

	template<typename T>
	class optional {
	public:
		typedef optional<T>	this_type;
		typedef T			value_type;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef T&&			rval_reference;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		
	public:
		optional()
			: d_init(false){
		}
		optional(const T& value){
			construct(value);
		}
		optional(T&& value){
			construct(std::move(value));
		}
		template<typename U>
		optional(const optional<U>& other){
			d_init = other.is_initialized();
			if (d_init) {
				construct(other.get());
			}
		}
		template<typename U>
		optional(optional<U>&& other) {
			d_init = other.is_initialized();
			if (d_init) {
				construct(std::move(other.get()));
			}
		}
		optional& operator=(const T& value) {
			destroy();
			construct(value);
			return *this;
		}
		optional& operator=(T&& value) {
			destroy();
			construct(std::move(value));
			return *this;
		}
		template<typename U>
		optional& operator=(const optional<U>& other) {
			destroy();
			d_init = other.is_initialized();
			if (d_init) {
				construct(other.get());
			}
			return *this;
		}
		template<typename U>
		optional& operator=(optional<U>&& other) {
			destroy();
			d_init = other.is_initialized();
			if (d_init) {
				construct(std::move(other.get()));
			}
			return *this;
		}
		optional<T>& swap(optional<T>& other) {
			std::swap(*this, other);
			return *this;
		}
		template<typename ... Args>
		optional& emplace(Args&& ... args) {
			destroy();
			new (&d_data) T(std::forward<Args>(args)...);
			d_init = true;
			return *this;
		}
		~optional() {
			destroy();
		}
		const_reference get() const {
			return get_ref();
		}
		reference get() {
			return get_ref();
		}
		const_pointer operator->() const {
			return get_ptr();
		}
		pointer operator ->() {
			return get_ptr();
		}
		const_pointer operator *() const {
			return get_ref();
		}
		pointer operator *() {
			return get_ref();
		}		
		const_reference value() const {
			if (d_init) {
				return get_ref();
			}else {
				throw bad_optional_access();
			}
		}
		reference value() {
			if (d_init) {
				return get_ref();
			}
			else {
				throw bad_optional_access();
			}
		}
		void reset() {
			destroy();
		}
		bool operator !() const{
			return !d_init;
		}
		explicit operator bool() const
		{
			return !this->operator!(); 
		}
		void destroy() {
			if (d_init) {
				get_ref().~T();
				d_init = false;
			}
		}
		bool is_initialized() const { 
			return d_init;
		}
	private:
		void construct(const T& value) {
			new (&d_data) T(value);
			d_init = true;
		}
		void construct(T&& value) {
			new (&d_data) T(std::move(value));
			d_init = true;
		}
		pointer get_ptr() {
			return d_init ? (pointer)(&d_data):0;
		}
		const_pointer get_ptr() const {
			return d_init ? (const_pointer)(&d_data):0;
		}
		reference get_ref() {
			return *get_ptr();
		}
		const_reference get_ref() const {
			return *get_ptr();
		}
	private:
		bool d_init = false;
		typename std::aligned_storage<sizeof(T), alignof(T)>::type d_data;
	};
	template<class T>
	class optional<T&&>
	{
		static_assert(sizeof(T) == 0, "Optional rvalue references are illegal.");
	};
	template<class T>
	class optional<T&>
	{
		static_assert(sizeof(T) == 0, "Optional lvalue references are illegal.");
	};
}
#endif
