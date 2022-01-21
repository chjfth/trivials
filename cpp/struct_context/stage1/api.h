#ifndef __api_h
#define __api_h

struct SA
{
	int ma;

	void *context;
};

struct SB
{
	int mb;

	void *context;
};

#define DEFINE_CONTEXT_SETTER(SetterFunc, BodyType, ContextType) \
	void SetterFunc(BodyType *body, ContextType *ctx) \
	{ \
		(body->context) = ctx; \
	} \

#define DEFINE_CONTEXT_GETTER(GetterFunc, BodyType, ContextType) \
	ContextType * GetterFunc(BodyType *body) \
	{ \
		return (ContextType*)(body->context); \
	} \

// SActx * pactx = getcontext(&sa, SActx);

#endif
