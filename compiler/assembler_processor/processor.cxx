//#define NDEBUG
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "stack.hxx"
#include "assembler_processor.hxx"
#include "processor.hxx"

#define PRINTF(...) printf(__VA_ARGS__)
#define FOPEN(file, name, type) { FILE* errcode = (file = fopen(name, type)); \
	 assert(errcode); }
	 
#define FCLOSE(file)	\
{						\
	fclose(file);		\
	file = NULL;		\
}

//=============================================================
Processor :: Processor(FILE* asm_file):
		ip_(0),
		sp_(0),
		file_size_(0)
{
	memset(registers_, 0, REGISTERS_QT * sizeof(int));
	read_asm_file(asm_file);
}
//=============================================================
Processor :: ~Processor() {
	memset(registers_, 0, REGISTERS_QT * sizeof(int));
	memset(prog_mem_, -1, (file_size_ + 1) * sizeof(char));
	free(prog_mem_);
	prog_mem_ = NULL;
	ip_ = 0;
	sp_ = 0;
}
//=============================================================
void Processor :: read_asm_file(FILE* asm_file) {
	file_size_ = get_file_size(asm_file);
	prog_mem_ = (char*)calloc(file_size_ + 1, sizeof(char));//file_size_ + 1 для '\0'
	fread(prog_mem_, sizeof(char), file_size_ + 1, asm_file);
}
//=============================================================
int Processor :: get_arg_qt(int num) {
#define DEF_CMD(name, argqt, code)		\
	if(num == ASM_##name)				\
		return argqt;
#include "cmds.hxx"
#undef DEF_CMD
	return -1;
}
//=============================================================
void Processor :: move_ip() {
	sp_ += CMD_LEN + (ARG_TYPE_LEN + ARG_LEN) * get_arg_qt(cmd_);
	ip_++;
}

void Processor :: move_ip(int new_ip) {
	sp_ = 0;
	char* present_scanf_ptr = NULL;
	for(int i = 0; i < new_ip; i++) {
		present_scanf_ptr = prog_mem_ + sp_;
		int errcode = sscanf(present_scanf_ptr, "%d", &cmd_);
		assert(errcode);
		sp_ += CMD_LEN + (ARG_TYPE_LEN + ARG_LEN) * get_arg_qt(cmd_);
	}
	ip_ = new_ip;
}
//=============================================================
void Processor :: execute() {
	char* present_scanf_ptr = NULL;
	while(sp_ + 1 < file_size_) {	// sp_ + 1 потому что всегда в конце файла есть один пробел
		present_scanf_ptr = prog_mem_ + sp_;
		if(!sscanf(present_scanf_ptr, " %d ", &cmd_)) {
			printf("\nInvalid command № %d. Only numbers allowed.\n", ip_);
			exit(1);
		}
		int arg_qt = get_arg_qt(cmd_);
		int arguments[MAX_ARG_QT]= {};
        int arg_types[MAX_ARG_QT] = {};
		for(int i = 0; i < arg_qt; i++) { 
			if(!sscanf(present_scanf_ptr + CMD_LEN + (ARG_TYPE_LEN + ARG_LEN) * i, " %d %d ",arg_types + i, arguments + i)) {
			printf("\nIvalid arguments in command № %d.\n", ip_);
			exit(1);
			}
		}
		bool fuckTheProcessor = false;
		switch(cmd_) {
#define DEF_CMD(name, argqt, code)		\
			case ASM_##name : {code}	\
			break;							
#include "cmds.hxx"
#undef DEF_CMD
			default: 
			{
				printf("\nINVALID COMMAND # %d\n", ip_);
				exit(1);
			}
		}
		if (fuckTheProcessor) break;
	}
}
//==============================================================
void Processor :: dump() {
	stack_dump();
	reg_dump();
	printf("%cOTHER INFORMATION\n{", '#');
	printf("\n\tsp_ = %d\n\tip_ = %d\n", sp_, ip_);
	printf("\tcmd is %d\n", cmd_);
	printf("\tfile_size is %d\n", file_size_);
	printf("\tprog_mem_ is\n");
	printf("\t%s\n", prog_mem_);
	printf("}\n");
}
//===============================================================
int Processor :: get_file_size(FILE* f) {
	if(!f) {
		printf("Can't read assembler file");
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	int i = ftell(f);
	rewind(f);
	return i;
}
//===============================================================
void Processor :: reg_dump() {
	printf("%cREGISTERS INFORMATION\n", '#');
	printf("registers_ptr is %p\n", registers_);
	for(int i = 0; i < 16; i++) 
		printf("\treg[%d] = %d\n", i, registers_[i]);
}

void Processor :: stack_dump() {
	printf("\n%cSTACK INFORMATION\n", '#');
	stack_.Dump_("stack_");
}
//=============================================================
#undef FOPEN
#undef FCLOSE
#undef PRINTF
