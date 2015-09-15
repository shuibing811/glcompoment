#ifndef ANY_H_
#define ANY_H_

#include <typeinfo>

namespace gl {
	class any {
	public:
		any()
			: d_content(nullptr){
		}
		any(const any & other)
			: d_content(other.d_content? other.d_content->clone():nullptr){
		}
		any(any && other)
			: d_content(other.d_content) {
			other.d_content = nullptr;
		}
		template<typename T> 
		any(T && value,typename std::enable_if<!std::is_same<any&,T>::value>::type* = nullptr/*disable any&*/,
					typename std::enable_if<!std::is_const<T>::value>::type* = nullptr/*disable const any&&*/ ): 
				d_content(new holder<typename std::decay<T>::type>(std::forward<T>(value)))  {
		}
		any & operator=(const any & other) {
			any(other).swap(*this);
			return *this;
		}
		any & operator=(any && other) {
			any(std::move(other)).swap(*this);
			return *this;
		}
		template<typename T> 
		any & operator=(T && other) {
			any(std::forward<T>(other)).swap(*this);
			return *this;
		}
		~any() {
			delete d_content;
		}

		any & swap(any & other) {
			std::swap(this->d_content, other.d_content);
			return *this;
		}

		bool empty() const {
			return d_content == nullptr;
		}
		void clear() {
			any().swap(*this);
		}
		const std::type_info & type() const {
			return d_content?d_content->type():typeid(void);
		}
	private:
		class placeholder {
		public:
			virtual ~placeholder() {};
			virtual placeholder* clone() const = 0;
			virtual const std::type_info& type() const = 0;
		};
		template<typename T>
		class holder : public placeholder {
		public:
			holder(const T& value) :d_value(value) {

			}
			holder(T&& value) :d_value(std::move(value)) {

			}
			virtual placeholder* clone() const {
				return new holder(d_value);
			}
			virtual const std::type_info& type() const {
				return typeid(d_value);
			}
		public:
			T	d_value;
		private:
			holder& operator=(const holder&);
		};
	private:
		placeholder* d_content;
	private:
		template<typename T>
		friend T * any_cast(const any *);

		template<typename T>
		friend T any_cast(const any &);
	};
	inline void swap(any & left, any & right) {
		left.swap(right);
	}
	class bad_any_cast : public std::bad_cast {
	public:
		virtual const char * what() const noexcept {
			return "boost::bad_any_cast: "
				"failed conversion using boost::any_cast";
		}
	};
	template<typename T>
	T* any_cast(const any* value) {
		if (!value || value->type() != typeid(T)) {
			return nullptr;
		}
		auto content = dynamic_cast<any::holder<typename std::decay<T>::type>*>(const_cast<any*>(value)->d_content);
		return &content->d_value;
	}
	template<typename T> 
	T any_cast(const any & value) {
		typedef typename std::remove_reference<T>::type noref;
		noref* result = any_cast<noref>(&value);
		if (!result) {
			throw bad_any_cast();
		}
		typedef typename std::conditional<std::is_reference<T>::value, T, typename std::add_lvalue_reference<T>::type>::type reftype;
		return static_cast<reftype>(*result);
	}
}
#endif
