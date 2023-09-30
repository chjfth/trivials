#ifndef __unp_autofree_h_
#define __unp_autofree_h_

#include <memory>
#include <type_traits>

#define MakeUniquePtrClass(UnpClassName, CleanupFunction, PTR_TYPE) \
	class UnpClassName ## _deleter { \
	public: \
		void operator() (PTR_TYPE ptr) { \
			CleanupFunction(ptr); \
		} \
	}; \
	static PTR_TYPE UnpClassName ## _dullptr; \
	typedef std::unique_ptr< std::remove_reference<decltype(*UnpClassName ## _dullptr)>::type, UnpClassName ## _deleter > UnpClassName;



#endif
