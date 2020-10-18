#ifndef __CircBufws_h_20161123_
#define __CircBufws_h_20161123_

#include <commdefs.h>

/*
Circular buffer with shadow, first used with UFCOM:

Feature:
 * 
 * All Put() is atomic.
 * Extra shadow space for peek-at-head and retrieve-at-end.

*/

class CCircBufws 
{
public:
	enum { Gap1Ele=1 };

	enum ReCode_et 
	{
		E_Success = 0,
		E_Fail = -1,
		E_BadParam = -2,
		E_NoMem = -3,
		E_Full = -4,
		E_OverwriteNotAllow = -5,
	};

	enum ResetPtr_et { NoReset=0, WantReset=1 };

public:
	CCircBufws();
	~CCircBufws();

	ReCode_et Init(int QueueEles, int ShadowEles=0);
		// Each ele is one byte.

	void Destroy(); // inversion of Init()

	int Put(const void *pdata, int ieles);
		// Put eles into circbuf, atomic operation.
		// return ieles put(eles or 0), or error code.
		// Special: If pdata==NULL, do a virtual put(=no copy data).

	int PutOverwrite(const void *pdata, int eles);
		// Put eles into circbuf, overwrite old data if full.
		// return eles overwritten , or error code.
		// Note: Don't use shadow op if you use overwrite-operation.

	int OverwriteHead(const void *pdata, int eles);

	int Get(void *pdata, int eles);
		// return eles get, or error code
		// Can return less than current eles.

	int Clear(int eles=-1, ResetPtr_et reset=NoReset);
		// -1 means clear all, or error code

	int Truncate(int eles_to_keep);

	int QSize() const { return m_qsize; }

	int StockEles() const;

	int VacantEles();

	void *HeadPtr(){ return m_pHead; }
	void *TailPtr_(){ return m_pTail_; }

	Uchar *MoveCheck(Uchar *start, int offset); // know start+offset's result with rewind

	int ShadowSize(){ return m_shadow_size; }

	ReCode_et ShadowPeekHead(int eles_to_peek, void **pHeadPtr);

	ReCode_et ShadowRetrieveEnd(int eles_to_retrieve, void **pTailWas=NULL, int *pOverWrittenBytes=NULL); 
		// will update m_pTail_, 
		// *pTailWas returns the tail-ptr before update.
		// If pOverWrittenBytes!=NULL, it will allow head-overwritten and outputs overwritten bytes.

	bool IsCreated() const { return m_qsize ? true: false; }

	int PeakUsage() const { return m_peakuse; }
	void ResetPeakUsage() { m_peakuse = 0; }

	bool ResetPtr();

private:
	ReCode_et _BreakApart(Uchar *pstart, int eles_count, int *part1, int *part2);

	void _ResetPtr();
	void _ResetPtrIfEmpty();

	int _AllStorage() { return m_qsize + m_shadow_size; }

	Uchar *qboundary_() { return m_qbuf+m_qsize + Gap1Ele; }

	Uchar *_MovePtrForward(Uchar *pstart, int eles);

	void _UpdatePeakUsage();

private:

	int m_qsize; // not counting the gap ele

	int m_shadow_size;

	Uchar *m_qbuf;
	Uchar *m_pHead;
	Uchar *m_pTail_; // points to one byte beyond the final filled byte

	int m_peakuse; // queue buffer peak usage

#ifdef DBG
	int m_cureles; // current elements
#endif
};


int CircBuf_MoveData(CCircBufws &src, CCircBufws &dst, int bytesToCopy=-1); // -1 means copy most bytes
	// Return bytes actually copied. -1 on fail.


///////////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION CODE BELOW ////////////////////////
///////////////////////////////////////////////////////////////////////////

#ifdef CircBufws_IMPL

#include <stdlib.h>

#include <_MINMAX_.h>
#include <EnsureClnup_common.h>

CCircBufws::CCircBufws()
{
	memset(this, 0, sizeof(*this));
}

CCircBufws::~CCircBufws()
{
	delete m_qbuf;
	m_qbuf = NULL;
}

CCircBufws::ReCode_et 
CCircBufws::Init(int QueueEles, int ShadowEles)
{
	// Can be called from un-initialized state

	if(QueueEles<=0 || ShadowEles<0)
		return E_BadParam;

	if(ShadowEles==0)
		ShadowEles = 1;

	m_qbuf = new Uchar[QueueEles+ShadowEles]; // no need to +Gap1Ele here
	if(!m_qbuf)
		return E_NoMem;

	m_pTail_ = m_pHead = m_qbuf;

	m_qsize = QueueEles;
	m_shadow_size = ShadowEles;

	memset(m_qbuf, 0, _AllStorage());

	return E_Success;
}

void 
CCircBufws::Destroy()
{
	delete m_qbuf;
	memset(this, 0, sizeof(*this));
}

void 
CCircBufws::_ResetPtr()
{
	m_pTail_ = m_pHead = m_qbuf;
}

void 
CCircBufws::_ResetPtrIfEmpty()
{
	if(m_pHead==m_pTail_)
		m_pTail_ = m_pHead = m_qbuf;
}

bool 
CCircBufws::ResetPtr()
{
	if(m_pHead = m_pTail_)
	{
		_ResetPtr();
		return true;
	}
	else
		return false;
}

Uchar * 
CCircBufws::_MovePtrForward(Uchar *pstart, int eles)
{
	assert(pstart>=m_pHead && pstart<qboundary_());
	assert(eles<=m_qsize);

	pstart += eles;
	if(pstart>=qboundary_())
		pstart -= (m_qsize+1);
	
#if DBG
	m_cureles -= eles;
#endif
	return pstart;
}

void 
CCircBufws::_UpdatePeakUsage()
{
	int now_eles = StockEles();
	if(now_eles>m_peakuse)
		m_peakuse = now_eles;
}

int 
CCircBufws::Put(const void *pdata_, int eles)
{
	if(eles<0)
		return E_BadParam;
	if(eles>VacantEles())
		return E_Full;

	if(eles==0)
		return 0;

	_ResetPtrIfEmpty();

	const Uchar *pdata = (Uchar*)pdata_;

	int part1, part2;
	_BreakApart(m_pTail_, eles, &part1, &part2);
	
	if(pdata)
		memcpy(m_pTail_, pdata, part1);

	if(part2==0)
	{
		m_pTail_ += part1;
		assert( m_pTail_<=qboundary_() );

		if(m_pTail_==qboundary_())
		{
			m_pTail_ = m_qbuf;
		}
	}
	else
	{
		if(pdata)
			memcpy(m_qbuf, pdata+part1, part2);
		
		m_pTail_ = m_qbuf + part2;
	}

#ifdef DBG
	m_cureles += eles;
#endif

	_UpdatePeakUsage();

	return eles;
}

int 
CCircBufws::PutOverwrite(const void *pdata_, int eles)
{
	if(eles<0)
		return E_BadParam;
	if(m_shadow_size>1)
		return E_OverwriteNotAllow; // bcz it uses _ResetPtr

	const Uchar *userdata = (Uchar*)pdata_;
	int overwrites = 0;

	if(eles<=VacantEles())
	{
		Put(userdata, eles);
	}
	else if(eles<m_qsize)
	{
		overwrites = eles - VacantEles();
		m_pHead = _MovePtrForward(m_pHead, overwrites);
		
		Put(userdata, eles);
		
		if(m_pHead==m_qbuf)
			assert(m_pTail_==m_qbuf+m_qsize);
		else
			assert(m_pHead==m_pTail_+1);
	}
	else // all stock eles are flushed out
	{
		overwrites = StockEles() + eles - m_qsize;
	
		_ResetPtr();
		memcpy(m_qbuf, userdata+eles-m_qsize ,m_qsize);

#if DBG
		m_cureles = m_qsize;
#endif
		m_peakuse = QSize();
	}

	return overwrites;
}

int 
CCircBufws::OverwriteHead(const void *pdata_, int eles)
{
	if(eles<0)
		return E_BadParam;
	if(m_shadow_size>1)
		return E_OverwriteNotAllow; // bcz it uses _ResetPtr

	const Uchar *userdata = (Uchar*)pdata_;
	int overwrites = 0;

	if(eles<StockEles())
	{
		int part1, part2;
		_BreakApart(m_pHead, eles, &part1, &part2);

		memcpy(m_pHead, userdata, part1);

		if(part2>0)
			memcpy(m_qbuf, userdata+part1, part2);
	}
	else
	{
		int exceed = eles-m_qsize;
		if(exceed>0)
		{
			userdata += exceed;
			eles -= exceed;
			overwrites += exceed;
		}

		overwrites += StockEles();

		_ResetPtr();

		memcpy(m_pHead, userdata, eles);
		m_pTail_ = m_pHead+eles;

#ifdef DBG
		m_cureles = eles;
#endif
	}

	return overwrites;
}


int 
CCircBufws::Get(void *pdata_, int eles)
{
	if(eles<0)
		return E_BadParam;

	int eles_to_copy = _MIN_(eles, StockEles()); // so eles can be larger than Qsize
	if(eles_to_copy==0)
		return 0;

	Uchar *pdata = (Uchar*)pdata_;

	int part1, part2;
	_BreakApart(m_pHead, eles_to_copy, &part1, &part2);

	if(pdata)
		memcpy(pdata, m_pHead, part1);

	if(part2==0)
	{
		m_pHead += part1;
		assert( m_pHead<=qboundary_() );

		if(m_pHead==qboundary_())
		{
			m_pHead = m_qbuf;
		}
	}
	else
	{
		if(pdata)
			memcpy(pdata+part1, m_qbuf, part2);
		
		m_pHead = m_qbuf + part2;
	}

#ifdef DBG
	m_cureles -= eles_to_copy;
#endif
	return eles_to_copy;
}

int 
CCircBufws::Clear(int eles, ResetPtr_et reset)
{
	if(eles<0)
	{	// clear all
		int orig = StockEles();
		
		if(reset)
			m_pTail_ = m_pHead = m_qbuf;
		else
		{
			// Note: Tail ptr may be in a pending USB read,
			// so we should move head ptr instead.
			m_pHead = m_pTail_;
		}

#ifdef DBG
		m_cureles = 0;
#endif
		return orig;
	}
	else
	{
		int ret = Get(NULL, eles);
		return ret;
	}
}

int 
CCircBufws::StockEles() const
{
	if(!IsCreated())
		return 0;

	int n = (int)(m_pTail_ - m_pHead);
	if(n<0)
		n += QSize() + Gap1Ele;

#ifdef DBG
	assert(n==m_cureles);
#endif
	return n;
}

int 
CCircBufws::VacantEles()
{
	return m_qsize - StockEles();
}

Uchar * 
CCircBufws::MoveCheck(Uchar *Start, int Offset)
{
	// Move Offset bytes from Start, considering rewind, see where we'll get.
	// Offset may be positive or negative.
	assert(Start>=m_qbuf && Start<qboundary_());
	assert(Offset>=-m_qsize && Offset<=m_qsize);

	Uchar *end1 = Start+Offset;

	if(Offset>=0 && end1>=qboundary_())
		return end1 - m_qsize - Gap1Ele;
	else if(Offset<0 && end1<m_qbuf)
		return end1 + m_qsize + Gap1Ele;
	else
		return end1;
}

CCircBufws::ReCode_et 
CCircBufws::ShadowPeekHead(int eles_to_peek, void **pHeadPtr)
{
	// Usage case: 
	// When user want to peek eles_to_peek bytes at the queue head and those bytes
	// happen to be rewinded, this function copies involved bytes to shadow area to
	// make for continuous eles_to_peek bytes.

	if(eles_to_peek>m_shadow_size || eles_to_peek<0)
		return E_BadParam;

	_ResetPtrIfEmpty();
	*pHeadPtr = m_pHead;

	int part1, part2;
	_BreakApart(m_pHead, eles_to_peek, &part1, &part2);

	if(part2>0)
	{
		memcpy(qboundary_(), m_qbuf, part2); // shadow <- real
	}

	return E_Success;
}

CCircBufws::ReCode_et 
CCircBufws::ShadowRetrieveEnd(int eles_to_retrieve, void **pTailWas, int *pOverWrittenBytes)
{
	// Usage case:
	// When user has stored eles_to_retrieve bytes to the queue tail(some of the bytes
	// may have been written to the shadow area), user can call ShadowRetrieveEnd()
	// to retrieve those out-of-boundary(=in shadow) bytes back to queue's real buffer.

	if(eles_to_retrieve>m_shadow_size || eles_to_retrieve<0)
		return E_BadParam;

	int overflow_bytes = eles_to_retrieve - VacantEles();

	if(!pOverWrittenBytes && overflow_bytes>0)
		return E_Full;

//	_ResetPtrIfEmpty(); // this is Wrong!

	if(pTailWas)
		*pTailWas = m_pTail_;

	int part1, part2;
	_BreakApart(m_pTail_, eles_to_retrieve, &part1, &part2);

	if(part2>0) // rewind
	{
		memcpy(m_qbuf, qboundary_(), part2); // real <- shadow
		m_pTail_ = m_qbuf+part2;
	}
	else // not rewind
	{
		m_pTail_ += part1;
		assert( m_pTail_<=qboundary_() );

		if(m_pTail_==qboundary_())
		{
			m_pTail_ = m_qbuf;
		}
	}

	Uchar *orighead = m_pHead; (void)orighead;

	if(overflow_bytes>0)
	{
		if( qboundary_()-1 == m_pTail_ )
			m_pHead = m_qbuf;
		else
			m_pHead = m_pTail_+1; 
	}

	if(pOverWrittenBytes)
		*pOverWrittenBytes = overflow_bytes>0 ? overflow_bytes : 0;

#ifdef DBG
	m_cureles += eles_to_retrieve;
	if(overflow_bytes>0)
	{
		Uchar *newhead = MoveCheck(orighead, overflow_bytes);
		assert(m_pHead==newhead);
		m_cureles -= overflow_bytes;
	}
#endif

	_UpdatePeakUsage();

	return E_Success;
}

CCircBufws::ReCode_et 
CCircBufws::_BreakApart(Uchar *pstart, int eles_count, int *part1, int *part2)
{
	// part1 is at queue space-tail, part2 rewinds to queue space-head.

	int part1_vacant = (int)( qboundary_() - pstart );

	assert(part1_vacant<=m_qsize+1);
	assert(eles_count<=m_qsize);

	if(part1_vacant>=eles_count)
	{	// no rewind store required.
		*part1 = eles_count;
		*part2 = 0;
	}
	else
	{	// rewind required.
		*part1 = part1_vacant;
		*part2 = eles_count - part1_vacant;
	}
	return E_Success;
}



int 
CircBuf_MoveData(CCircBufws &src, CCircBufws &dst, int reqCopy)
{
	if(reqCopy==0)
		return 0;

	int stockSrc = src.StockEles();
	int vacantDst = dst.VacantEles();

	int trueCopy = _MIN_(stockSrc, vacantDst);

	if(reqCopy>0)
	{
		trueCopy = _MIN_(trueCopy, reqCopy);
	}

	if(trueCopy==0)
		return 0;

	Cec_delete_pchar tmpbuf = new char[trueCopy];
	if(!tmpbuf)
		return -1;

	// CCircBufws::ReCode_et qerr = 
	int ret = src.Get(tmpbuf, trueCopy);
	assert(ret==trueCopy);

	ret = dst.Put(tmpbuf, trueCopy);
	assert(ret==trueCopy);
	
	return trueCopy;
}


#endif // CCIRCBUFWS_IMPL
#endif // __CircBufws_h_20161123_
