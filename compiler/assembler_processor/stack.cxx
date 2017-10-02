#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#define NDEBUG

#include "stack.hxx"
//======================================================================
Stack :: Stack() {
	capacity_  = STACK_CAP;
	size_  = 0;
	for(int i = 0; i < capacity_; i++)
		data_[i] = -666;
	summ_ = get_summ();
}
//======================================================================
Stack :: ~Stack() {
	for(int i = 0; i < capacity_; i++)
		data_[i] = 0;
	size_ = 0;
	capacity_ = 0;
	summ_ = 0;
}
//======================================================================
STKERRS Stack :: is_ok() {
	#define PUTERR(condition, name)  \
	if(condition) {                 \
        printf(#condition);         \
        return STKERR_##name;       \
    }
	PUTERR(!this, NULLPTR)
	PUTERR(!data_, DATANULLPTR)
	PUTERR(size_ > capacity_, BIGSIZE)
//	PUTERR(summ_ != get_summ(), BADSUMM)
	#undef PUTERR
	return STKERR_NO;
}
//======================================================================
void Stack :: Dump_(const char stkname[MAXLEN]) {
	printf("Stack %s is %s\n", stkname, !is_ok() ? "OK" : "NOT OK");
	printf("{\n");
	printf("\tcapacity_ is     %d\n", capacity_);
	printf("\tsize_ is         %d\n",     size_);
	printf("\tsumm_ is         %d\n",     summ_);
	printf("\tdata_ is         %p\n",     data_);
	for(int i = 0; i < capacity_; i++) 
		printf("\t\t%c [%d]: %d\n", (i < size_) ? '*' : ' ', i, data_[i]);
	printf("\n");
	printf("}\n");
}
#define Dump(stkptr)						\
{											\
	assert(strlen(#stkptr) < MAXLEN);		\
	Dump_(stkptr, #stkptr);					\
}
//======================================================================
int Stack :: push(int value) {
	if(is_ok()) {
		printf("PUSH ERROR - STACK IS NOT OK\n");
		return 0;
	}
	if(size_ == capacity_) {
		printf("PUSH ERROR - STACK IS FULL");
	}
	data_[size_++] = value;
	summ_ = get_summ();
	return 1;
}
//======================================================================
#define POPPEEK(namefunc, number, errormessage)	\
int Stack :: namefunc() {			\
	if(is_ok()) {					\
		printf(errormessage);		\
		exit(1);					\
	}								\
	if (size_ == 0){				\
		printf("\nNO ELEMENTS IN STACK. PROGRAM ABORT\n");\
		exit(1);					\
	}								\
	int res = data_[number];		\
	summ_ = get_summ();				\
	return res;						\
}

POPPEEK(pop, --size_, "POP ERROR - STACK IS NOT OK\n")
POPPEEK(peek, size_ - 1, "PEEK ERROR - STACK IS NOT OK\n")
//======================================================================
int Stack :: get_summ() {
	int i;
	int check_sum = 0;
	for(i = 0; i < size_; i++) 
		check_sum += data_[i];
	check_sum += size_;
	check_sum += capacity_;
	return check_sum;
}
