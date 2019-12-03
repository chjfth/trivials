#include <stdio.h>

#define SEE_CASE1_ERROR
#define SEE_CASE2_ERROR
#define SEE_CASE3_ERROR

//////////////// CASE 0 ////////////////

class A {};
class B : public A {};

void test_case0()
{
	A *pa = 0;
	B *pb = 0;
	pa = pb; // OK
//	pb = pa; // of course error.
}

//////////////// CASE 1 ////////////////

template<typename T>
class Compound
{
	T *ptr;
};

void test_case1()
{
	Compound<A> *pca = 0;
	Compound<B> *pcb = 0;
#ifdef SEE_CASE1_ERROR
	pca = pcb; // Oops, not allowed.
#endif
}

//////////////// CASE 2 ////////////////

typedef void UseA(A *pa);
typedef void UseB(B *pb);

void test_case2()
{
	UseA *afunc = NULL;
	UseB *bfunc = NULL;

#ifdef SEE_CASE2_ERROR
	afunc = bfunc; // Error
#endif
}

//////////////// CASE 3 ////////////////

template<typename T>
class CompoundPTR
{
	void         (*func1)(Compound<T> *cptr);
	Compound<T>* (*func2)(int);
};

void test_case3()
{
	CompoundPTR<A> *ca_ptr = 0;
	CompoundPTR<B> *cb_ptr = 0;

#ifdef SEE_CASE3_ERROR
	ca_ptr = cb_ptr;
#endif
}

int main()
{
	return 0;
}

