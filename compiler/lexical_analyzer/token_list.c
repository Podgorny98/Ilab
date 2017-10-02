#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../enums"
#include "token_list.h"

struct token_list * token_ctor() {
    struct token_list * tl = (struct token_list*)malloc(sizeof(struct token_list));
    tl->first = tl->last = NULL;
    return tl;
}
//=========================================================================
void token_push_ch(struct token_list * list, enum TOKEN_TYPES Type, char* Data, int len, int Str_Num) {
    struct token_node * new_token = (struct token_node*)malloc(sizeof(struct token_node));
    new_token->type = Type;
    new_token->data.ch = (char*)malloc((len + 1) * sizeof(char));
    strncpy(new_token->data.ch, Data, len);
    new_token->data.ch[len] = '\0';
    new_token->str_num = Str_Num;
    new_token->next = NULL;
    new_token->prev = list->last;
    if(!list->first)
        list->first = list->last = new_token;
    else {
        list->last->next = new_token;
        list->last = new_token;
    }
}
//=========================================================================
void token_push_i(struct token_list * list, enum TOKEN_TYPES Type, int Data, int Str_Num) {
    struct token_node * new_token = (struct token_node*)malloc(sizeof(struct token_node));
    new_token->type = Type;
    new_token->data.i = Data;
    new_token->str_num = Str_Num;
    new_token->next = NULL;
    new_token->prev = list->last;
    if(!list->first)
        list->first = list->last = new_token;
    else {
        list->last->next = new_token;
        list->last = new_token;
    }
}
//=========================================================================
void token_detor(struct token_list * list) {
    while(list->first) {

        struct token_node * cur_node = list->first;
        list->first = cur_node->next;
        if(cur_node->type == TYPE_IDENTIFIER) {
            free(cur_node->data.ch);
 
            cur_node->data.ch = NULL;
        }
        else {
            cur_node->data.i = -666;
        }
        cur_node->type = TYPE_ERROR;
        cur_node->next = NULL;
        cur_node->prev = NULL;
        free(cur_node);
        cur_node = NULL;
    }
    list->first = list->last = NULL;
    free(list);
    list = NULL;
}
//=========================================================================
void token_list_dump(struct token_list  * list) {
    FILE* dump_file;
    dump_file = fopen("dot_dump_file.gv", "w");
    fprintf(dump_file,"digraph graf{\n"); 
    token_node_dump(list->first,dump_file); 
    fprintf(dump_file,"}"); 
    fclose(dump_file); 
    system ("dot dot_dump_file.gv -Tpng -o dot_dump_file.png");
    system ("xdot dot_dump_file.gv");
}
//===================================================================
#define FPRINTF_TOKEN_STR(Type, Data)                           \
    if(cur_node->type == Type && cur_node->data.i == Data) {    \
        fprintf(dump_file, "type = "                            \
                            #Type                               \
                            "\\ldata = "                        \
                            #Data                               \
                            "\"]\n");                           \
    }

void token_node_dump(struct token_node *  cur_node, FILE* dump_file) { 
   // printf("\n%d_%d", cur_node->type, cur_node->data.i);

    fprintf (dump_file, "treeNode_%p [label=\"", cur_node);
    if(cur_node->type == TYPE_IDENTIFIER) {
        fprintf(dump_file, "type = TYPE_IDENTIFIER\\l"
                        "data = %s\"]\n", cur_node->data.ch);
    }
    else if(cur_node->type == TYPE_CONST) {
        fprintf(dump_file, "type = TYPE_CONST\\l"
                        "data = %d\"]\n", cur_node->data.i);
    }
    else {
        #include "fprintf_token_str"
    }

    if(cur_node->next) {
        fprintf (dump_file, "treeNode_%p->treeNode_%p\n",
                                cur_node, cur_node->next);
        token_node_dump (cur_node->next, dump_file); 
    }
}

#undef FPRINTF_TOKEN_STR







