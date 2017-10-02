#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "assembler_processor.hxx"
#include "assembler.hxx"
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
//=============================================================
Assembler :: Assembler(FILE* in, FILE* out):
	fin(in),
	fout(out),
	outstr{}
{
    for(int i = 0; i < MAX_LABELS_QT; i++)
        labels[i] = NULL;
    memset(labels_nums, -1, MAX_LABELS_QT * sizeof(int));
    labels_qt = 0;
}
//=============================================================
Assembler :: ~Assembler() {
    for(int i = 0; i < labels_qt; i++) {
        free(labels[i]);
        labels[i] = NULL;
    }
    for(int i = 0; i < labels_qt; i++)
        labels_nums[i] = -1;
    labels_qt = 0;
}
//=============================================================
void Assembler :: printf_out() {
	fprintf(fout, "%s", outstr);
    FCLOSE(fin);
    FCLOSE(fout)
}
//=============================================================
ASM_CMDS Assembler :: get_num(const char* cmdstr) {
#define DEF_CMD(name, argqt, code)	                \
	if(!strcmp(#name, cmdstr))	                    \
		return ASM_##name;
#include "cmds.hxx"
#undef DEF_CMD
	return ASM_DEFAULT;
}
//=============================================================
int Assembler :: get_arg_qt(ASM_CMDS num) {
#define DEF_CMD(name, argqt, code)					\
	if(num == ASM_##name)						    \
		return argqt;
#include "cmds.hxx"
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
	char cur_str[MAX_CMD_LEN] = {};
	int com_num = 0; //Счётчик команд
	while(!feof(fin)) {
		FSCANF(fin, " %s", cur_str)
		if(*cur_str == ':') {
            assert(strlen(cur_str) >= 2);
            if (cur_str[1] == '\0') {
                printf("\nWRONG LABEL NAME AFTER COMMAND №%d\n", com_num);
                exit(1);
            }
			for(int i = 0; i < labels_qt; i++) {
                if(!strcmp(cur_str + 1, labels[i])) {
				    printf("\nASSEMBLER CODE ERROR: EQUAL LABELS\n");
				    exit(1);
			    }
            }
            
            labels[labels_qt] = (char*)malloc(strlen(cur_str) * sizeof(char));
            strcpy(labels[labels_qt], cur_str + 1);
			labels_nums[labels_qt] = com_num;
            labels_qt++;
		}
		else if(*cur_str == ';')
			fscanf(fin, "%*[^\n]");
		else {
			fscanf(fin, "%*[^\n]");
			com_num++;
		}
	}
}
//====================================================================
void Assembler :: second_passing() {
	rewind(fin);
	int offset = 0;
	int com_num = 0;	//номер команды по ходу чтения файла
	while(!feof(fin)) {
		int printf_len = -1;
		char cur_str[MAX_CMD_LEN] = {};
		fscanf(fin, " %s", cur_str);
		if(*cur_str == ':')
			fscanf(fin, "%*[^\n]");
		else if(*cur_str == ';')
			fscanf(fin, "%*[^\n]");
		else {
			ASM_CMDS cmd_num = get_num(cur_str);
			if(cmd_num == ASM_DEFAULT) {
				printf("\ncommand name %s is wrong in command № %d. assembler abort.\n", cur_str, com_num);
				return;
			}
			sprintf(outstr + offset, "%03d %n", cmd_num, &printf_len);
			offset += printf_len;
			get_args(cmd_num, com_num, &offset);
		    com_num++;
		}
	fscanf(fin, "%*[ \t\r\n]");	//пропускаем все пробельные символы 
	}							//после прочтения команды и аргументов
}
//==================================================================================
void Assembler :: get_args(ASM_CMDS cmd_num, int com_num, int* offset) {
	int arg_qt = get_arg_qt(cmd_num);
    for(int i = 0; i < arg_qt; i++) {
        int arg;
        char arg_type;
        int printf_len = -1;
        if((cmd_num >= ASM_JG && cmd_num <= ASM_JNE && i == 2) || 
           (cmd_num == ASM_JMP) || (cmd_num == ASM_CALL)) {
            char label_name[MAX_CMD_LEN];
            fscanf(fin, "%s", label_name);
           // printf("%s\n", label_name);

            if(*label_name != ':'){
                printf("\nInvalid label in the (C)JMP in command № %d\n", com_num);
                printf("%s\n", label_name);
                exit(1);
            }
            bool undefined_label = true;
            int label_num;
            for(int j = 0; j < labels_qt; j++) {
                if(!strcmp(labels[j], label_name + 1)) {
                    undefined_label = false;
                    label_num = j;
                }
            }
            if(undefined_label) {
                 printf("\nUndefined label is used in command № %d. Assembler abort.\n", com_num);
                 exit(1);
            }
            
            sprintf(outstr + *offset, "%d %+010d %n", ARG_LABEL, labels_nums[label_num], &printf_len);
        }
        else {
            FSCANF(fin, " %c", &arg_type)
            FSCANF(fin, "%d", &arg)
            switch(arg_type) {
                case '%':
                    arg_type = ARG_REG;
                    break;
                case '$':
                    arg_type = ARG_NUMBER;
                    break;
                case '[':
                {
                    char c = '\0';
                    fscanf(fin, "%c", &c);
                    if(c != ']') {
                        printf("__%c__", c);
                        printf("\nInvalid arg_type in command № %d. Assembler abort.\n", com_num);
                        exit(1);
                    }
                    arg_type = ARG_STACK;
                }; break;
                default:
                    printf("\nInvalid arg_type in command № %d. Assembler abort.\n", com_num);
                    exit(1);
            }
            sprintf(outstr + *offset, "%d %+010d %n", arg_type, arg, &printf_len);
            if(printf_len == -1)
            {
                printf("\nInvalid argument in command № %d. Assembler abort.\n", com_num);
                return;
            }
        }
        *offset += printf_len;
    }
}






#undef FOPEN
#undef FCLOSE
#undef FSCANF
