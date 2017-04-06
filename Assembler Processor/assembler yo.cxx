//#define NDEBUG
#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define FOPEN(file, name, type) { FILE* errcode = (file = fopen(name, type)); \
	 assert(errcode); }
	 
#define FCLOSE(file)	\
{						\
	fclose(file);		\
	file = NULL;		\
}
#define FSCANF(...) {	\
	int errcode = fscanf(__VA_ARGS__);	\
	assert(errcode);					\
}

enum {MAXLEN = 1024};
enum {LABELS_QT = 10};
enum {MAX_ARG_QT = 4};
enum {MAX_CMD_LEN = 10};
enum {MAX_FILE_NAME = 30};

enum ASM_CMDS {
#define DEF_CMD(cmdname, argqt, code) \
	ASM_##cmdname,
#include "cmds.cxx"
#undef DEF_CMD
	ASM_DEFAULT
};

class Assembler {
private:
	FILE* fin;
	FILE* fout;
	int labels[LABELS_QT];
	int get_num(const char* cmdstr);
	int get_arg_qt(ASM_CMDS code);
	void first_passing();
	void second_passing();
public:
	Assembler(FILE* in, FILE* out);
	~Assembler();
	void printf_out();
	void file_analisis();
	char outstr[MAXLEN];
};
//=============================================================
int main()
{
	FILE* in;
	FILE* out;
	char input_file_name[MAX_FILE_NAME] = {};
	char output_file_name[MAX_FILE_NAME] = {};
	printf("Please, input name of in-file\n");
	scanf("%29s", input_file_name);
	printf("\nPlease, input name of out-file\n");
	scanf("%29s", output_file_name);
	FOPEN(in, input_file_name, "r");
	FOPEN(out, output_file_name, "w");
	Assembler a(in, out);
	a.file_analisis();
	a.printf_out();
	return 0;
}
//=============================================================
Assembler :: Assembler(FILE* in, FILE* out):
	fin(in),
	fout(out),
	outstr{}
{
	memset(labels, -1, LABELS_QT * sizeof(int));
}
//=============================================================
Assembler :: ~Assembler() {
	FCLOSE(fin);
	FCLOSE(fout);
}
//=============================================================
void Assembler :: printf_out() {
	fprintf(fout, "%s", outstr);
}
//=============================================================
int Assembler :: get_num(const char* cmdstr) {
#define DEF_CMD(name, argqt, code)	\
	if(!strcmp(#name, cmdstr))	\
		return ASM_##name;
#include "cmds.cxx"
#undef DEF_CMD
	return ASM_DEFAULT;
}
//=============================================================
int Assembler :: get_arg_qt(ASM_CMDS num) {
#define DEF_CMD(name, argqt, code)					\
	if(num == ASM_##name)						\
		return argqt;
#include "cmds.cxx"
#undef DEF_CMD
	return -1;
}
//==============================================================
void Assembler :: file_analisis() {
	first_passing();
	second_passing();
}
//===================================================================
void Assembler :: first_passing() {
	char present_str[MAX_CMD_LEN] = {};
	int comNum = 0; //Счётчик команд
	while(!feof(fin)) {
		FSCANF(fin, " %s", present_str)
		if(*present_str == ':') {
			if ((!isdigit(present_str[1])) || (strlen(present_str) != 2) )
			{
				printf("\nInvalid label %s. Only :[0-9] labels allowed\n", 
									present_str);
				exit(1);
			}
			if(labels[present_str[1] - '0'] != -1) {
				printf("\nASSEMBLER CODE ERROR: EQUAL LABELS\n");
				exit(1);
			}
			labels[present_str[1] - '0'] = comNum;
		}
		else if(*present_str == ';')
			fscanf(fin, "%*[^\n]");
		else {
			fscanf(fin, "%*[^\n]");
			comNum++;
		}
	}
}
//====================================================================
void Assembler :: second_passing() {
	rewind(fin);
	int offset = 0;
	int comNum = 0;	//номер команды по ходу чтения файла
	while(!feof(fin)) {
		int printf_len = -1;
		char present_str[MAX_CMD_LEN] = {};
		int arguments[MAX_ARG_QT] = {};
		char argtype = 0;
		FSCANF(fin, " %s", present_str)
		if (*present_str == ':')
			fscanf(fin, "%*[^\n]");
		else if(*present_str == ';')
			fscanf(fin, "%*[^\n]");
		else {
			ASM_CMDS cmdNum = (ASM_CMDS)get_num(present_str);
			if(cmdNum == ASM_DEFAULT) {
				printf("\nCommand name %s is wrong in command № %d. Assembler abort.\n", present_str, comNum);
				return;
			}
			sprintf(outstr + offset, "%03d %n", cmdNum, &printf_len);
			offset += printf_len;
			int arg_qt = get_arg_qt(cmdNum);
			for(int i = 0; i < arg_qt; i++) {
				printf_len = -1;
				FSCANF(fin, " %c", &argtype)
				FSCANF(fin, "%d", arguments + i)
				if((cmdNum >= ASM_JG && cmdNum <= ASM_JNE && i == 2) || 
				   (cmdNum == ASM_JMP) || (cmdNum == ASM_CALL)) {
					if((arguments[i] < 0) || (arguments[i] > 9)) {
						printf("\nInvalid label in the (C)JMP in command № %d\n", comNum);
						return;
					}
					if(labels[arguments[i]] == -1) {
						printf("\nUndefined label is used in command № %d. Assembler abort.\n", comNum);
						return;
					}
					sprintf(outstr + offset, "%+010d %n", labels[arguments[i]], &printf_len);
				}
				else
					sprintf(outstr + offset, "%+010d %n", arguments[i], &printf_len);
				if(printf_len == -1)
				{
					printf("\nInvalid argument in command № %d. Assembler abort.\n", comNum);
					return;
				}
				offset += printf_len;
			}
		comNum++;
		}
	fscanf(fin, "%*[ \t\n]");	//пропускаем все пробельные символы 
	}							//после прочтения команды и аргументов
}










