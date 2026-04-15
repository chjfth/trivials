// This class obeys my ct0r class style.

class CA
{
public:
	// boilerplate code, no need to modify >>>
	CA() {	_ct0r(); }
	virtual ~CA()
	{ 
		_dtor(); 
		_ct0r();
	}
	CA(const CA& old)            // copy-ctor
	{
		printf("CA.copy-ctor %p\n", this);
		_copy_from_old(old); 
	}
	CA& operator=(const CA& old) // copy-assign
	{
		printf("CA.copy-assign %p -> %p\n", &old, this);
		if (this != &old) {
			_dtor();
			_copy_from_old(old);
		}
		return *this;
	}
	CA(CA&& old)            // move-ctor
	{
		printf("CA.move-ctor %p\n", this);
		_steal_from_old(old);
		old._ct0r();
	}
	CA& operator=(CA&& old) // move-assign
	{
		printf("CA.move-assign %p -> %p\n", &old, this);
		if (this != &old) {
			_dtor();
			_steal_from_old(old);
			old._ct0r();
		}
		return *this;
	}
	// boilerplate code, no need to modify <<<

private:
	void _copy_from_old(const CA& old) {
		if(old.m_buf)
			m_buf = new char[1], m_buf[0] = old.m_buf[0];
		else 
			m_buf = nullptr;
	}

	void _steal_from_old(CA& old) {
		m_buf = old.m_buf;
	}

public:
	// ... more public methods here ...

	//
	// Leave below at end of class body
	//
private: // data members
	char* m_buf;

private:
	void _ct0r() {
		m_buf = nullptr;
	}

	void _dtor() {
		delete[] m_buf;
	}
};

