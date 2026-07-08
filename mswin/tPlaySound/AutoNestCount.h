#pragma once

#include <InterpretConst.h>


//////// WM_xxx tracking code >>>>

extern int g_nestlv; // WM_xxx nested level
extern int g_msgcount;
extern bool g_inside_getmessage;

namespace itc {
extern const CInterpretConst& itc_WM_app_all();
}

inline const TCHAR* get_indents(int nestlv)
{
	static TCHAR s_indents[40];

	int i;
	for(i=0; i<nestlv-1 && i<ARRAYSIZE(s_indents)-1; i++)
		s_indents[i] = ' ';

	s_indents[i] = '\0';
	return s_indents;
}

class AutoNestCount
{
public:
	AutoNestCount(int& msgcount, int& nestlv, UINT msg) 
		: mr_msgcount(msgcount), mr_nestlv(nestlv), m_msg(msg)
	{ 
		mr_msgcount++;
		m_nowcount = mr_msgcount;

		mr_nestlv++;
		DumpOne(true);
	}
	~AutoNestCount()
	{ 
		DumpOne(false);
		mr_nestlv--;
	}

private:
	void DumpOne(bool is_enter)
	{
		vaDbgTs(_T("<%c>[#%-3d]%s[*%d] %s %s"), 
			g_inside_getmessage ? _T('i') : _T('o') , // <%c> : 'i/o': inside/outsiude GetMessage()
			m_nowcount, get_indents(mr_nestlv), mr_nestlv, // [#%d]%s[*%d]
			is_enter ? _T(">>> Enter") : _T("<<< Leave") , // %s : Enter or Leave
			ITCSnv(m_msg, itc::itc_WM_app_all) // %s WM_xxx name and value
			);
	}
private:
	int& mr_msgcount;
	int& mr_nestlv;
	int m_nowcount;
	UINT m_msg;
};

//////// WM_xxx tracking code <<<<