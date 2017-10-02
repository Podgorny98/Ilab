#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../enums"
#include "../syntactical_analyzer/syntactical_tree.h"
#include "semantic_analyzer.h"

static FILE* asm_file;

#define FPRINTF(str) fprintf(asm_file, str);
#define FPRINTF1(str, arg) fprintf(asm_file, str, arg);
#define FPRINTF2(str, arg1, arg2) fprintf(asm_file, str, arg1, arg2);

//===============================================================
void semantic_analyzer(tree_node* syntactical_tree) {
    asm_file = fopen("asm_file", "w");
    FPRINTF("CALL :main\n")
    FPRINTF("HALT\n\n\n\n")
    for(int i = 0; i < syntactical_tree->child_qt; i++)
        translate_function(syntactical_tree->children[i]);
    fclose(asm_file);
}
//===============================================================
int get_arg_var_offset(struct func_description s, char* var_name) {
    for(int i = 0; i < s.arg_qt; i++) {
        if(!strcmp(s.args[i], var_name))
            return -4 - i;
    }
    for(int i = 0; i < s.var_qt; i++) {
        if(!strcmp(s.vars[i], var_name))
            return i;
    }
}
//===============================================================
struct func_description func_description_ctor(tree_node* func_node) {
    struct func_description s;
    s.name = func_node->data.ch;
    s.arg_qt = func_node->children[0]->child_qt;
    s.var_qt = func_node->children[1]->child_qt;
    
    for(int i = 0; i < s.arg_qt; i++)
        s.args[i] = func_node->children[0]->children[i]->data.ch;
    
    for(int i = 0; i < s.var_qt; i++)
        s.vars[i] = func_node->children[1]->children[i]->data.ch;
    
    return s;
}

//===============================================================
void translate_function(tree_node* cur_node) {
    struct func_description cur_function = func_description_ctor(cur_node);
    FPRINTF1(":%s\n", cur_node->data.ch)
    FPRINTF("PUSH %%12\n")
    FPRINTF("PUSH %%14\n")
    FPRINTF("MOV %%14 %%15\n")
    FPRINTF1("ADD %%15 $%d\n", cur_function.var_qt);             //place for variables
    
    for(int i = 0; i < cur_node->children[2]->child_qt; i++)
        translate_operator(cur_node->children[2]->children[i], cur_function);
    
    if(cur_node->children[2]->children[cur_node->children[2]->child_qt - 1]->type != RETURN) {
        FPRINTF("MOV %%15 %%14\n")
        FPRINTF("POP %%14\n")
        FPRINTF("POP %%12\n")
        FPRINTF("RET\n")
    }
    FPRINTF("\n\n\n");
}
//===============================================================
 void translate_operator(tree_node* cur_node, struct func_description cur_function) {
     switch(cur_node->type) {
        case FUNCTION:
            translate_func_call(cur_node, cur_function);
            break;

        case IF:
            translate_if(cur_node, cur_function);
            break;

        case WHILE:
            translate_while(cur_node, cur_function);
            break;

        case OPERATOR:
            translate_expr(cur_node, cur_function);
            break;

        case SCAN:
            translate_scan(cur_node, cur_function);
            break;

        case PRINT:
            translate_print(cur_node, cur_function);
            break;

        case RETURN:
            translate_return(cur_node, cur_function);
            break;
    }
}
//===============================================================
void translate_func_call(tree_node* cur_node, struct func_description cur_function) {
    FPRINTF("MOV %%12 %%15\n")
    translate_func_call_args(cur_node, cur_function);
    FPRINTF1("CALL :%s\n", cur_node->data.ch)
    FPRINTF("MOV %%15 %%12\n")
}
//===============================================================
void translate_func_call_args(tree_node* cur_node, struct func_description cur_function) {
    int arg_qt = cur_node->child_qt;
    for(int i = arg_qt - 1; i >=  0; i--) {
        if(cur_node->children[i]->type == CONST)
            FPRINTF1("PUSH $%d\n", cur_node->children[i]->data.i)
        else {
            int offset = get_arg_var_offset(cur_function, cur_node->children[i]->data.ch);
            FPRINTF1("PUSH [%d]\n", offset)
        }
    }
}
//===============================================================
void translate_if(tree_node* cur_node, struct func_description cur_function) {
    static int first_free_loop = 0;
    int cur_if_else_loop = first_free_loop;
    first_free_loop++;
    condition_jump(cur_node->children[0], cur_function);
    FPRINTF1(":if%d\n", cur_if_else_loop)
    for(int i = 0; i < cur_node->children[1]->child_qt; i++)
        translate_operator(cur_node->children[1]->children[i], cur_function);
    if(cur_node->children[2]) {
        FPRINTF1("JMP :else%d\n", cur_if_else_loop)
        FPRINTF1(":if%d\n", cur_if_else_loop)
        for(int i = 0; i < cur_node->children[2]->child_qt; i++)
            translate_operator(cur_node->children[2]->children[i], cur_function);
        FPRINTF1(":else%d\n", cur_if_else_loop)
    }
    else
        FPRINTF1(":if%d\n", cur_if_else_loop)
}
//===============================================================
void condition_jump(tree_node* condition_node, struct func_description cur_function) {
    if(condition_node->children[0]->type == CONST)
        FPRINTF1("MOV %%0 $%d\n", condition_node->children[0]->data.i)
    else {
        int offset = get_arg_var_offset(cur_function, condition_node->children[0]->data.ch);
        FPRINTF1("MOV %%0 [%d]\n", offset)
    }
    
    if(condition_node->children[1]->type == CONST)
        FPRINTF1("MOV %%1 $%d\n", condition_node->children[1]->data.i)
    else {
        int offset = get_arg_var_offset(cur_function, condition_node->children[1]->data.ch);
        FPRINTF1("MOV %%1 [%d]\n", offset)
    }
    switch(condition_node->data.i) {
        case OPERATOR_EQUAL:
            FPRINTF("JNE ")
            break;

        case OPERATOR_NOT_EQUAL:
            FPRINTF("JE ")
            break;

        case OPERATOR_LARGER:
            FPRINTF("JLE ")
            break;

        case OPERATOR_LARGER_EQUAL:
            FPRINTF("JL ")
            break;

        case OPERATOR_LESS:
            FPRINTF("JGE ")
            break;

        case OPERATOR_LESS_EQUAL:
            FPRINTF("JG ")
            break;
    }
    FPRINTF("%%0 %%1 ")
}
//===============================================================
void translate_while(tree_node* cur_node, struct func_description cur_function) {
    static int first_free_loop = 0;
    int first_while_loop = first_free_loop;
    int second_while_loop = first_while_loop + 1;
    first_free_loop += 2;
    FPRINTF1(":while%d\n", first_while_loop)
    condition_jump(cur_node->children[0], cur_function);
    FPRINTF1(":while%d\n", second_while_loop)
    for(int i = 0; i < cur_node->children[1]->child_qt; i++)
        translate_operator(cur_node->children[1]->children[i], cur_function); 
    FPRINTF1("JMP :while%d\n", first_while_loop)
    FPRINTF1(":while%d\n", second_while_loop)
}
//===============================================================
void translate_scan(tree_node* cur_node, struct func_description cur_function) {
    int offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
    FPRINTF1("IN [%d]\n", offset)
}
//===============================================================
void translate_print(tree_node* cur_node, struct func_description cur_function) {
    if(cur_node->children[0]->type == VARIABLE) {
        int offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
        FPRINTF1("OUT [%d]\n", offset)
    }
    else if(cur_node->children[0]->type == CONST)
        FPRINTF1("OUT $%d\n", cur_node->children[0]->data.i)
}
//===============================================================
void translate_return(tree_node* cur_node, struct func_description cur_function) {
    if(cur_node->children[0]->type == CONST)
        FPRINTF1("MOV %%13 $%d\n", cur_node->children[0]->data.i)
    else if(cur_node->children[0]->type == VARIABLE) {
        int offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
        FPRINTF1("MOV %%13 [%d]\n", offset);
    }

    FPRINTF("MOV %%15 %%14\n")
    FPRINTF("POP %%14\n")
    FPRINTF("POP %%12\n")
    FPRINTF("RET\n")
}
//===============================================================
void translate_expr(tree_node* cur_node, struct func_description cur_function) {
    int assign_var_offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
    int second_var_offset;

    switch(cur_node->children[1]->type) {
        case CONST:                                     // a = 1;
            FPRINTF2("MOV [%d] $%d\n", assign_var_offset, cur_node->children[1]->data.i);
            break;

        case FUNCTION:                                  // a = f(z);
            translate_func_call(cur_node->children[1], cur_function);
            FPRINTF1("MOV [%d] %%13\n", assign_var_offset)
            break;

        case VARIABLE:                                  // a = b;
            second_var_offset = get_arg_var_offset(cur_function, cur_node->children[1]->data.ch);
            FPRINTF2("MOV [%d] [%d]\n", assign_var_offset, second_var_offset)
            break;

        case OPERATOR:
            translate_math_op(cur_node->children[1], cur_function);
            FPRINTF1("POP [%d]\n", assign_var_offset)
            break;
    }
}
//===============================================================
char* get_math_com_name(int com_num) {
    switch(com_num) {
        case OPERATOR_ADD:
            return "ADD";
        case OPERATOR_SUB:
            return "SUB";
        case OPERATOR_MUL:
            return "MUL";
        case OPERATOR_DIV:
            return "DIV";
    }
}
//===============================================================
void translate_math_op(tree_node* cur_node, struct func_description cur_function) {
    char* op_name = get_math_com_name(cur_node->data.i);
    int offset;
    
    if(cur_node->children[1]->type != OPERATOR && cur_node->children[1]->type != FUNCTION) {
        switch(cur_node->children[0]->type) {
            case VARIABLE:
                offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
                FPRINTF1("MOV %%0 [%d]\n", offset)
                break;

            case CONST:
                FPRINTF1("MOV %%0 $%d\n", cur_node->children[0]->data.i)
                break;

            case FUNCTION:
                translate_func_call(cur_node->children[0], cur_function);
                FPRINTF("MOV %%0 %%13\n")
                break;

            case OPERATOR:
                translate_math_op(cur_node->children[0], cur_function);
                FPRINTF("POP %%0\n")
                break;
        }
        
        switch(cur_node->children[1]->type) {
            case VARIABLE:
                offset = get_arg_var_offset(cur_function, cur_node->children[1]->data.ch);
                FPRINTF2("%s %%0 [%d]\n", op_name, offset)
                break;

            case CONST:
                FPRINTF2("%s %%0 $%d\n", op_name, cur_node->children[1]->data.i)
                break;
        }
    }
    else {
        switch(cur_node->children[0]->type) {
            case VARIABLE:
                offset = get_arg_var_offset(cur_function, cur_node->children[0]->data.ch);
                FPRINTF1("PUSH [%d]\n", offset)
                break;

            case CONST:
                FPRINTF1("PUSH $%d\n", cur_node->children[0]->data.i)
                break;

            case FUNCTION:
                translate_func_call(cur_node->children[0], cur_function);
                FPRINTF("PUSH %%13\n")
                break;

            case OPERATOR:
                translate_expr(cur_node->children[0], cur_function);
                break;
        }

        switch(cur_node->children[1]->type) {
            case FUNCTION:
                translate_func_call(cur_node->children[1], cur_function);
                FPRINTF("POP %%0\n")
                FPRINTF1("%s %%0 %%13\n", op_name)
                break;

            case OPERATOR:
                translate_math_op(cur_node->children[1], cur_function);
                FPRINTF("POP %%1\n")
                FPRINTF("POP %%0\n")
                FPRINTF1("%s %%0 %%1\n", op_name)
                break;
        }
    }
    FPRINTF("PUSH %%0\n")
}







#undef FPRINTF2
#undef FPRINTF1
#undef FPRINTF
