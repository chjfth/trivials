#ifndef __api_h
#define __api_h

#include <string>
#include <map>

using namespace std;

struct SA
{
	int ma;

	map<string, void*> ctxmap;
};

struct SB
{
	int mb;

	map<string, void*> ctxmap;
};


#define DEFINE_CONTEXT_SETTER(SetterFunc, BodyType, ContextType) \
	void SetterFunc ## BodyType(BodyType *body, ContextType *ctx, const char *ctxstr) \
	{ \
		body->ctxmap.insert(make_pair(ctxstr, ctx)); \
	} \
	void SetterFunc(BodyType *body, ContextType *ctx) \
	{ \
		SetterFunc ## BodyType(body, ctx, #ContextType); \
	}

#define DEFINE_CONTEXT_GETTER(GetterFunc, BodyType, ContextType) \
	ContextType * GetterFunc ## BodyType(BodyType *body, const char *ctxstr) \
	{ \
		return (ContextType*)(body->ctxmap[ctxstr]); \
	} \
	ContextType * GetterFunc(BodyType *body) \
	{ \
		return GetterFunc ## BodyType(body, #ContextType); \
	}


#endif
