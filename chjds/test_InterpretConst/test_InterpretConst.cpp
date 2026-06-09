#include <CHHI_DEBUG.h>
//
#include <ps_TCHAR.h>
#include <msvc_extras.h>
#include <commdefs.h>
#include <chj_mishmash.h>
#include <sdring.h>

#include <InterpretConst.h>
using namespace itc;


const Enum2Val_st _e2v_weekday[] =
{
	{_T("Sunday"), 0},
	{_T("Monday"), 1},
	{_T("Tuesday"), 2},
	{_T("Wednesday"), 3},
	{_T("Thursday"), 4},
	{_T("Friday"), 5},
	{_T("Saturday"), 6},
};
CInterpretConst itc_weekday(_e2v_weekday);

void test_weekday()
{
	const TCHAR *answer = nullptr;
	int DiffAt = 0;
	Sdring res;

	int arVals[] = { 0,1,2,3,4, 5,6,7 };
	int i;

	res.set_empty();
	for (i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"),
			arVals[i], ITCS(arVals[i], itc_weekday));
	}
	answer = _T("\
  0 : Sunday\n\
  1 : Monday\n\
  2 : Tuesday\n\
  3 : Wednesday\n\
  4 : Thursday\n\
  5 : Friday\n\
  6 : Saturday\n\
  7 : 7\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));

	res.set_empty();
	for (i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"),
			arVals[i], ITCSv(arVals[i], itc_weekday));
	}
	answer = _T("\
  0 : 0(Sunday)\n\
  1 : 1(Monday)\n\
  2 : 2(Tuesday)\n\
  3 : 3(Wednesday)\n\
  4 : 4(Thursday)\n\
  5 : 5(Friday)\n\
  6 : 6(Saturday)\n\
  7 : 7\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));
}

//////////////////////////////////////////////////////////////////////////

const Enum2Val_st _e2v_Enumgroup1[] =
{
	{ _T("G1_VAL0"), 0 },
	{ _T("G1_VAL1"), 1 },
	{ _T("G1_VAL2"), 2 },
	{ _T("G1_VAL3"), 3 },
};
const Enum2Val_st _e2v_Enumgroup2[] =
{
	{ _T("G2_VAL0"), 0 << 2 },
	{ _T("G2_VAL1"), 1 << 2 },
	{ _T("G2_VAL2"), 2 << 2 },
	{ nullptr,       3 << 2 }, // "0xC0" will be the interpreted string, as if this element is not mentioned
	{ _T("G2_VAL4"), 4 << 2 },
	{ _T("G2_VAL5"), 5 << 2 },
	{ _T("G2_VAL6"), 6 << 2 },
//	{ _T("G2_VAL7"), 7 << 2 },
};

const EnumGroup_st _egs1[] =
{
	{ 0x3,    _e2v_Enumgroup1, ARRAY_SIZE(_e2v_Enumgroup1) },
	{ 0x7<<2, _e2v_Enumgroup2, ARRAY_SIZE(_e2v_Enumgroup2) },
};

CInterpretConst itc_egs1(_egs1);

void test_EnumGroup()
{
	const TCHAR *answer = nullptr;
	int DiffAt = 0;
	Sdring res;

	int arVals[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 17, 0xff };
	int i;

	// ITCS()

	res.set_empty();
	for (i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"), 
			arVals[i], ITCS(arVals[i], itc_egs1));
	}
	answer = _T("\
  0 : G1_VAL0|G2_VAL0\n\
  1 : G1_VAL1|G2_VAL0\n\
  2 : G1_VAL2|G2_VAL0\n\
  3 : G1_VAL3|G2_VAL0\n\
  4 : G1_VAL0|G2_VAL1\n\
  5 : G1_VAL1|G2_VAL1\n\
  6 : G1_VAL2|G2_VAL1\n\
  7 : G1_VAL3|G2_VAL1\n\
  8 : G1_VAL0|G2_VAL2\n\
  9 : G1_VAL1|G2_VAL2\n\
 10 : G1_VAL2|G2_VAL2\n\
 11 : G1_VAL3|G2_VAL2\n\
 12 : G1_VAL0\n\
 16 : G1_VAL0|G2_VAL4\n\
 17 : G1_VAL1|G2_VAL4\n\
255 : G1_VAL3|0x1C|0xE0\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));

	// ITCSvn

	res.set_empty();
	for (i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"),
			arVals[i], ITCSvn(arVals[i], itc_egs1));
	}
	answer = _T("\
  0 : 0x0(G1_VAL0)|0x0(G2_VAL0)\n\
  1 : 0x1(G1_VAL1)|0x0(G2_VAL0)\n\
  2 : 0x2(G1_VAL2)|0x0(G2_VAL0)\n\
  3 : 0x3(G1_VAL3)|0x0(G2_VAL0)\n\
  4 : 0x0(G1_VAL0)|0x4(G2_VAL1)\n\
  5 : 0x1(G1_VAL1)|0x4(G2_VAL1)\n\
  6 : 0x2(G1_VAL2)|0x4(G2_VAL1)\n\
  7 : 0x3(G1_VAL3)|0x4(G2_VAL1)\n\
  8 : 0x0(G1_VAL0)|0x8(G2_VAL2)\n\
  9 : 0x1(G1_VAL1)|0x8(G2_VAL2)\n\
 10 : 0x2(G1_VAL2)|0x8(G2_VAL2)\n\
 11 : 0x3(G1_VAL3)|0x8(G2_VAL2)\n\
 12 : 0x0(G1_VAL0)\n\
 16 : 0x0(G1_VAL0)|0x10(G2_VAL4)\n\
 17 : 0x1(G1_VAL1)|0x10(G2_VAL4)\n\
255 : 0x3(G1_VAL3)|0x1C|0xE0\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));

	// ITCSnv

	res.set_empty();
	for (i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"),
			arVals[i], ITCSnv(arVals[i], itc_egs1));
	}
	answer = _T("\
  0 : G1_VAL0(0x0)|G2_VAL0(0x0)\n\
  1 : G1_VAL1(0x1)|G2_VAL0(0x0)\n\
  2 : G1_VAL2(0x2)|G2_VAL0(0x0)\n\
  3 : G1_VAL3(0x3)|G2_VAL0(0x0)\n\
  4 : G1_VAL0(0x0)|G2_VAL1(0x4)\n\
  5 : G1_VAL1(0x1)|G2_VAL1(0x4)\n\
  6 : G1_VAL2(0x2)|G2_VAL1(0x4)\n\
  7 : G1_VAL3(0x3)|G2_VAL1(0x4)\n\
  8 : G1_VAL0(0x0)|G2_VAL2(0x8)\n\
  9 : G1_VAL1(0x1)|G2_VAL2(0x8)\n\
 10 : G1_VAL2(0x2)|G2_VAL2(0x8)\n\
 11 : G1_VAL3(0x3)|G2_VAL2(0x8)\n\
 12 : G1_VAL0(0x0)\n\
 16 : G1_VAL0(0x0)|G2_VAL4(0x10)\n\
 17 : G1_VAL1(0x1)|G2_VAL4(0x10)\n\
255 : G1_VAL3(0x3)|0x1C|0xE0\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));
}

//////////////////////////////////////////////////////////////////////////

const SingleBit2Val_st _b2v_sample1[] =
{
	{ _T("bit0"), 1 << 0 },
	{ _T("bit1"), 1 << 1 },
	{ _T("bit2"), 1 << 2 },
	{ _T("bit4"), 1 << 4 },
	
	{ _T("bit5and6"), 32 + 64 }, // bit 5&6 must both be set to signify this name
	// -- Yes, although the struct is named SingleBit2Val_st, it can actually
	//    deal with an element with multiple bits.
};
CInterpretConst BFsample1(_b2v_sample1);

void test_Bitfield2Val()
{
	const TCHAR *answer = nullptr;
	int DiffAt = 0;
	Sdring res;

	int arVals[] = { 0,1,2,3,4,5,6,7,8,9, 16,17, 32,64,96 };
	for (int i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"), 
			arVals[i], ITCS(arVals[i], BFsample1));
	}
	answer = _T("\
  0 : 0\n\
  1 : bit0\n\
  2 : bit1\n\
  3 : bit0|bit1\n\
  4 : bit2\n\
  5 : bit0|bit2\n\
  6 : bit1|bit2\n\
  7 : bit0|bit1|bit2\n\
  8 : 0x8\n\
  9 : bit0|0x8\n\
 16 : bit4\n\
 17 : bit0|bit4\n\
 32 : 0x20\n\
 64 : 0x40\n\
 96 : bit5and6\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));
}

void test_bitfields_customfmt()
{
	CInterpretConst itc_bitfields_customfmt(_b2v_sample1, _T("0x%04x"));

	const TCHAR *answer = nullptr;
	int DiffAt = 0;
	Sdring res;

	int arVals[] = { 0,1,2,3,4,5,6,7,8,9, 16,17, 32,64,96 };
	for (int i = 0; i < ARRAY_SIZE(arVals); i++)
	{
		vaSdringAppendSelf(res, _T("%3d : %s\n"), 
			arVals[i], ITCSv(arVals[i], itc_bitfields_customfmt));
	}
	answer = _T("\
  0 : 0\n\
  1 : bit0(0x0001)\n\
  2 : bit1(0x0002)\n\
  3 : bit0(0x0001)|bit1(0x0002)\n\
  4 : bit2(0x0004)\n\
  5 : bit0(0x0001)|bit2(0x0004)\n\
  6 : bit1(0x0002)|bit2(0x0004)\n\
  7 : bit0(0x0001)|bit1(0x0002)|bit2(0x0004)\n\
  8 : 0x8\n\
  9 : bit0(0x0001)|0x8\n\
 16 : bit4(0x0010)\n\
 17 : bit0(0x0001)|bit4(0x0010)\n\
 32 : 0x0020\n\
 64 : 0x0040\n\
 96 : bit5and6(0x0060)\n\
");
	assert(Sdring::str_match(res, answer, &DiffAt));
}



int _tmain(int argc, TCHAR *argv[])
{
	MSVCRT_MemCheckStart(foo);

	test_weekday();
	test_EnumGroup();
	test_Bitfield2Val();
	test_bitfields_customfmt();

	bool isleak = MSVCRT_MemCheckEnd_IsLeak(foo);
	if (isleak) {
		printf("Memleak in my C++ code.\n");
		return 4;
	}
	else {
		printf("Success.\n");
		return 0;
	}
}
