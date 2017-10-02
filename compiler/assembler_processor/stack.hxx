enum { MAXLEN = 256 };
enum { CAP_CHANGE = 15 };
enum {STACK_CAP = 100000};
enum STKERRS {
	STKERR_NO,
	STKERR_NULLPTR,
	STKERR_DATANULLPTR,
	STKERR_BIGSIZE,
	STKERR_BADSUMM
};
class Stack {
	int capacity_;
	int summ_;
public:
    int size_;
	int data_[STACK_CAP];
	Stack();
	~Stack();
	void Dump_(const char stkname[MAXLEN]);
	int push(int value);
	int pop();
	int peek();
	STKERRS is_ok();
	int get_summ();
};
