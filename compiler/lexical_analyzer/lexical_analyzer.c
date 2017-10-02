#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#include "../enums"
#include "token_list.h"
#include "lexical_analyzer.h"


//===================================================================
#define DEF_CHECK_TOKEN(Str, Len, Type, Data)               \
    else if(!strncmp(Str, cur_token_ptr, Len)) {            \
        token_push_i(list, Type, Data, str_num + 1);        \
        cur_token_ptr += Len;                               \
        miss_space(&cur_token_ptr);                         \
        token_defined = 1;                                  \
    }                                                       
 
//===================================================================
void lexical_analyzer(FILE* my_file, struct token_list * list) {
    int file_size = get_file_size(my_file);
    char* my_prog = (char*)malloc((file_size + sizeof('\0')) * sizeof(char));
    fread(my_prog, sizeof(char), file_size, my_file);
    my_prog[file_size] = '\0';
    char* cur_token_ptr = NULL;
    
    delete_comments(my_prog, list);

    int str_qt = get_str_qt(my_prog);
    char** strings = (char**)malloc(str_qt * sizeof(char*));
    make_str_ptrs(my_prog, strings);
    for(int str_num = 0; str_num < str_qt; str_num++) {
        cur_token_ptr = strings[str_num];
        while(*cur_token_ptr != '\0') {
            miss_space(&cur_token_ptr);
            int token_defined = 0;
            if(0) {}
#include "def_check_token"
            if(!token_defined && (isalpha(*cur_token_ptr) || *cur_token_ptr == '_'))
                check_identifier(&cur_token_ptr, &token_defined, list, str_num);
            if(!token_defined && (('0' <= *cur_token_ptr && *cur_token_ptr <= '9') || *cur_token_ptr == '-'))
                check_number(&cur_token_ptr, &token_defined, list, str_num);
            if(!token_defined) {
                printf("\nERROR: invalid word \"%s\"\n", cur_token_ptr);
                exit(1);
            }
        }
    }
    free(strings);
    free(my_prog);
}
//===================================================================
void delete_comments(char* prog, struct token_list * list) {
    while(strstr(prog, "//")) {
        char* comment_start = strstr(prog, "//");
        char* comment_end;
        if(strstr(comment_start, "\n")) {
            comment_end = strstr(comment_start, "\n");
        }
        else {
            comment_end = prog + strlen(prog) - 1;
        }
        
        memset(comment_start, ' ', comment_end - comment_start);
    }
    while(strstr(prog, "/*")) {
        char* comment_start = strstr(prog, "/*");
        char* comment_end = strstr(comment_start, "*/");
        if(!comment_end) {
            printf("ERROR: unfinished comment");
            token_detor(list);
            exit(0);
        }
        memset(comment_start, ' ', comment_end - comment_start + 2);
    }
}
int get_file_size(FILE* f) {
	fseek(f, 0, SEEK_END);
	int i = ftell(f);
	rewind(f);
	return i;
}

//===================================================================
int get_str_qt(char* text) {
    int res = 1;
    int len = strlen(text);
    for(int i = 0; i < len; i++) { 
        if(text[i] == '\n')
                res++;
    }
    return res;
}
//===================================================================
void make_str_ptrs(char* text, char** str_ptrs) {
    str_ptrs[0] = text;
    int str_qt = 0;
    int len = strlen(text);
    for(int i = 0;i < len; i++) {
        if(text[i] == '\n') {
            text[i] = '\0';
            str_ptrs[++str_qt] = text + i + 1;
        }
    }
    for(int i = 0; i < str_qt; i++) {
        int empty_str = 1;
        len = strlen(str_ptrs[i]);
        for(int k = 0; k < len; k++) {
            if(str_ptrs[i][k] != ' ' && str_ptrs[i][k] != '\n' && str_ptrs[i][k] != '\t')
                empty_str = 0;
        }
        if(empty_str)
            str_ptrs[i][0] = '\0';
    }
}
//===================================================================
void miss_space(char** cur_token_ptr) {
    while(isspace(**cur_token_ptr))
        (*cur_token_ptr)++;
}
//===================================================================
void check_identifier(char** cur_token_ptr, int* token_defined, struct token_list * list, int str_num) {
    int i = 0;
    while(isalpha(*((*cur_token_ptr) + i)) || *((*cur_token_ptr) + i) == '_') 
        i++;
    token_push_ch(list, TYPE_IDENTIFIER, *cur_token_ptr, i, str_num + 1);
    *token_defined = 1;
    *cur_token_ptr += i;
    miss_space(cur_token_ptr);
}
//===================================================================
void check_number(char** cur_token_ptr, int* token_defined, struct token_list * list, int str_num) {
    int i = 0;
    if(**cur_token_ptr == '-')
        i++;
    while('0' <= *(*cur_token_ptr + i) && *(*cur_token_ptr + i) <= '9')
        i++;
    if(**cur_token_ptr == '0' && i > 1) {
        printf("\nInvalid number \"%s\"\n", *cur_token_ptr);
        exit(1);
    }
    char* end_ptr;
    long int num = strtol(*cur_token_ptr, &end_ptr, 10);
    if(errno == ERANGE || num > INT_MAX) {
        printf("\nERROR: Too big number %li\n", num);
        exit(1);
    }
    token_push_i(list, TYPE_CONST, (int)num, str_num + 1);
    *token_defined = 1;
    *cur_token_ptr += i;
    miss_space(cur_token_ptr);
}




#undef DEF_CHECK_TOKEN
