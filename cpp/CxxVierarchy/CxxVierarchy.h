#ifndef __CxxVierarchy_h_20250518_
#define __CxxVierarchy_h_20250518_

// This requires C++14 (VC2015+)
// Vierarchy: V-round-calling hierarchy

#include <tuple>
#include <utility>
#include <type_traits>

typedef signed int VSeq_t;

inline bool VSeq_IsBeforeChild(VSeq_t s) { return s <= 0; }
inline bool VSeq_IsAfterChild(VSeq_t s) { return s >= 0; }

// Helper: invoke member function with tuple unpacking (C++14 ok)
template <typename T, typename MemFn, typename Tuple, size_t... Indices>
void CVR_call_member_with_tuple(
	T* obj, MemFn memfn, VSeq_t vseq, Tuple&& t, std::index_sequence<Indices...>) 
{
	(obj->*memfn)(vseq, std::get<Indices>(std::forward<Tuple>(t))...);
}

template <typename T, typename MemFn, typename Tuple>
void CVR_call_member_with_tuple(
	T* obj, MemFn memfn, VSeq_t vseq, Tuple&& t) 
{
	constexpr size_t N = std::tuple_size<typename std::decay<Tuple>::type>::value;
	CVR_call_member_with_tuple(
		obj, memfn, vseq, std::forward<Tuple>(t), std::make_index_sequence<N>{});
}


// Main class
template <typename T, typename MemFn, typename... Args>
class VierachyCall {
public:
	VierachyCall(T* obj, MemFn memfn, VSeq_t vseq, Args&&... args)
		: object(obj), func(memfn), m_vseq(vseq), arguments(std::forward<Args>(args)...)
	{
		if(VSeq_IsBeforeChild(m_vseq))
			CVR_call_member_with_tuple(object, func, m_vseq-1, arguments);
	}

	~VierachyCall() 
	{
		if(VSeq_IsAfterChild(m_vseq))
			CVR_call_member_with_tuple(object, func, m_vseq+1, arguments);
	}

private:
	T* object;
	MemFn func;
	std::tuple<Args...> arguments;

	VSeq_t m_vseq;
};


template <typename T, typename MemFn, typename... Args>
auto MakeVierachyCall(T* obj, MemFn memfn, VSeq_t vseq, Args&&... args) {
	return VierachyCall<T, MemFn, Args...>(obj, memfn, vseq, std::forward<Args>(args)...);
}


#endif
