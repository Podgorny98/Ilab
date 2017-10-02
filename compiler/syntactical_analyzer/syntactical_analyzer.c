#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../enums"
#include "../lexical_analyzer/token_list.h"
#include "syntactical_tree.h"
#include "syntactical_analyzer.h"

static token_node* cur_token;
static tree_node* syntactical_tree_head;


struct function {
    char* name;
    int arg_qt;
}functions[MAX_FUNCTIONS_QT];

static int cur_func_num;
static int functions_qt;
static int is_main_searched;

char* cur_func_vars[MAX_VAR_QT];
int cur_func_vars_qt;
//================================================================
#define EXIT(message) {                 \
    printf(#message);                   \
    tree_dtor(syntactical_tree_head);   \
    exit(1);                            \
}

#define CHECK_TYPE_DATA(Type, Data) {                                           \
    if(cur_token->type != Type || cur_token->data.i != Data) {                  \
        printf("\n\nSyntactical error in line  %d \n\n", cur_token->str_num);   \
        tree_dtor(syntactical_tree_head);                                       \
        exit(1);                                                                \
    }                                                                           \
}

#define CHECK_TYPE(Type) {                                                  \
    if(cur_token->type != Type) {                                           \
        printf("\n\nSyntactical error in line %d\n\n", cur_token->str_num);     \
        tree_dtor(syntactical_tree_head);                                   \
        exit(1);                                                            \
    }                                                                       \
}

#define EXIT_STR_LINE(message, str, line) {                                 \
    printf(#message);                                                       \
    printf(" %s", str);                                                     \
    printf(" in line %d\n\n", line);                                        \
    tree_dtor(syntactical_tree_head);                                       \
    exit(1);                                                                \
}

#define EXIT_LINE(message, line) {                                          \
    printf(#message);                                                       \
    printf(" in line %d %s\n\n", line, cur_token->data.ch);                 \
    tree_dtor(syntactical_tree_head);                                       \
    exit(1);                                                                \
}

#define EXIT_STR(message, str) {                                            \
    printf(#message);                                                       \
    printf(" %s\n\n", str);                                                 \
    tree_dtor(syntactical_tree_head);                                       \
    exit(1);                                                                \
}
//================================================================
void next_token() {
   // if(!(cur_token->next))
    //    EXIT(\n\nERROR: wrong end of programm\n\n)
    cur_token = cur_token->next;
}
//================================================================
tree_node* syntactical_analyzer(struct token_list * list) {
   cur_token= list->first;
   tree_node* syntactical_tree = get_programm();
   cur_func_num = 0;
   functions_qt = 0;
   is_main_searched = 0;
   return syntactical_tree;
}
//================================================================
tree_node* get_programm() {
    tree_node* head_node = tree_node_ctor_i(NULL, CONNECTING_NODE, 0);
    syntactical_tree_head = head_node;
    while(cur_token) {
        if(functions_qt == MAX_FUNCTIONS_QT)
            EXIT(\n\nToo many functions in one programm\n\n)
        head_node->children[(head_node->child_qt)++] = get_function();
    }
    if(!is_main_searched)
        EXIT(\n\nERROR: No main in programm\n\n)
    return head_node;
}
//================================================================
tree_node* get_function() {
    cur_func_num = functions_qt;
    cur_func_vars_qt = 0;
    CHECK_TYPE_DATA(TYPE_KEY_WORD, KEY_WORD_FUNCTION)           //function
    next_token();                                               //funciton name
    if(cur_token->type == TYPE_KEY_WORD && cur_token->data.i == KEY_WORD_MAIN) {
        cur_token->data.ch = "main";
        is_main_searched = 1;
    }
    else
        CHECK_TYPE(TYPE_IDENTIFIER);
    check_only_function_declaration();          
    functions_qt++;
    tree_node* res = tree_node_ctor_ch(syntactical_tree_head, FUNCTION, cur_token->data.ch);
    functions[cur_func_num].name = res->data.ch;
    next_token();                                              //(
    res->children[0] = get_args();              //should put cur_function arg_qt,make connecting node,end at node {
    res->children[0]->parent = res;
    CHECK_TYPE_DATA(TYPE_KEY_WORD, KEY_WORD_START_OP_BLOCK)  //{
    next_token();                           //to vars declaration
    res->children[1] = get_new_vars();   //should make connecting node, end at start of function body
    res->children[1]->parent = res;
    res->children[2] = get_op_block();
    res->children[2]->parent = res;
    res->child_qt = 3;
    return res;
}
//================================================================
void check_only_function_declaration() {
    for(int i = 0; i < functions_qt; i++) {
        if(!strcmp(cur_token->data.ch, functions[i].name))
            EXIT_STR_LINE(\n\nERROR: second declaration of function, cur_token->data.ch, cur_token->str_num)
    }
}
//================================================================
void check_only_variable_declaration() {
    int i;
    for(i = 0; i < functions[cur_func_num].arg_qt; i++) {
        if(!strcmp(cur_token->data.ch, cur_func_vars[i]))
            EXIT_STR_LINE(\n\nERROR: argument and variable with same names in function, functions[cur_func_num].name, cur_token->str_num)
    }
    for(; i < cur_func_vars_qt; i++) {
        if(!strcmp(cur_token->data.ch, cur_func_vars[i]))
            EXIT_STR_LINE(\n\nERROR: second declaration of variable in function, functions[cur_func_num].name, cur_token->str_num)
    }
}
//================================================================
#define CHECK_FUNC_VAR_DECLARATION(name, qt, element)                                       \
int check_##name##_declaration() {                                                          \
    int not_declared = 1;                                                                   \
    int i;                                                                                  \
    for(i = 0; i < qt; i++) {                                                               \
        if(!strcmp(cur_token->data.ch, element)) {                                          \
            not_declared = 0;                                                               \
            break;                                                                          \
        }                                                                                   \
    }                                                                                       \
    if(not_declared)                                                                        \
        EXIT_STR_LINE(\n\nERROR: undeclared name, cur_token->data.ch, cur_token->str_num)       \
    return i;                                                                               \
}

CHECK_FUNC_VAR_DECLARATION(function, functions_qt, functions[i].name)
CHECK_FUNC_VAR_DECLARATION(variable, cur_func_vars_qt, cur_func_vars[i])

#undef CHECK_FUNC_VAR_DECLARATION
//================================================================
tree_node* get_args() {
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_OPEN)
    next_token();               //first var
    tree_node* res = tree_node_ctor_i(NULL, CONNECTING_NODE, 0);
    cur_func_vars_qt = 0;
    if(cur_token->type == TYPE_SKOBKI_KRYGLYE) {
        CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)
        next_token();           //{
        functions[cur_func_num].arg_qt = cur_func_vars_qt;
        return res;
    }
    while(1) {
        if(cur_func_vars_qt == MAX_ARGS_QT)
            EXIT_STR_LINE(\n\nERROR: Too many arguments in declaration of function, cur_token->data.ch, cur_token->str_num)
        CHECK_TYPE(TYPE_IDENTIFIER)
        res->children[(res->child_qt)++] = tree_node_ctor_ch(res, VARIABLE, cur_token->data.ch);
        cur_func_vars[cur_func_vars_qt++] = cur_token->data.ch;
        next_token();       // , or )
        if(cur_token->type != TYPE_ZAPYATAYA)
            break;
        next_token();       //next var
    }
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)
    functions[cur_func_num].arg_qt = cur_func_vars_qt;
    next_token();       // {
    return res;
}
//================================================================
tree_node* get_new_vars() {
    tree_node* res = tree_node_ctor_i(NULL, CONNECTING_NODE, 0);
    int vars_qt = 0;
    while(cur_token->type == TYPE_KEY_WORD && cur_token->data.i == KEY_WORD_INT) {
        next_token();   //to var name
        CHECK_TYPE(TYPE_IDENTIFIER)
        if(vars_qt == MAX_VAR_QT)
            EXIT_STR(\n\nERROR: Too many variables in function, functions[cur_func_num].name)
        check_only_variable_declaration();
        res->children[(res->child_qt)++] = tree_node_ctor_ch(res, VARIABLE, cur_token->data.ch);
        cur_func_vars[cur_func_vars_qt++] = cur_token->data.ch;
        next_token();           // to ;
        CHECK_TYPE(TYPE_TOCHKA_S_ZAPYATOI)
        next_token();           //to next string
        vars_qt++;
    }
    return res;
}
//================================================================
tree_node* get_op_block() {
    tree_node* res = tree_node_ctor_i(NULL, CONNECTING_NODE, 0);
    while(1) {
        if(res->child_qt == MAX_CHILDREN_QT)
            EXIT_STR(\n\nERROR:Too many operators in function, functions[cur_func_num].name)
        else if(cur_token->type == TYPE_KEY_WORD && cur_token->data.i == KEY_WORD_END_OP_BLOCK) {
           // if(cur_token->next)
            next_token();
            break;
        }
        res->children[res->child_qt] = get_operator();
        res->children[(res->child_qt)++]->parent = res;
    }
    return res;
}
//================================================================
tree_node* get_operator() {
    if(cur_token->type == TYPE_IDENTIFIER) {
        if(cur_token->next->type == TYPE_SKOBKI_KRYGLYE && cur_token->next->data.i == SKOBKA_OPEN)
           return get_func_call();
        else if(cur_token->next->type == TYPE_OPERATOR && cur_token->next->data.i == OPERATOR_ASSIGNMENT)
            return get_expr();
        
            EXIT_LINE(\n\nSyntactical error, cur_token->str_num)
    }
    else if(cur_token->type == TYPE_KEY_WORD) {
        switch(cur_token->data.i) {
            case KEY_WORD_SCANF:
                next_token();           // (
       //         printf("scanf\n");
                return get_scan();

            case KEY_WORD_PRINTF:
                next_token();           // (
        //        printf("printf\n");
                return get_print();

            case KEY_WORD_WHILE:
                next_token();           // (
         //       printf("whule\n");
                return get_while();

            case KEY_WORD_IF:
                next_token();           // (
         //       printf("if\n");
                return get_if();

            case KEY_WORD_RETURN:
                next_token();           // return | [a;]
        //        printf("return\n");
                return get_return();
            
            default:
                EXIT_LINE(\n\nSyntactical error, cur_token->str_num)
        }
   }
   else 
        EXIT_LINE(\n\nSyntactical error, cur_token->str_num)
}
//================================================================
#define GET_SCAN_PRINT(Name, Type)                              \
tree_node* get_##Name {                                         \
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_OPEN)           \
    next_token();                                               \
    CHECK_TYPE(TYPE_IDENTIFIER)                                 \
    tree_node* res = tree_node_ctor_i(NULL, Type, 0);           \
    res->children[0] = get_var();                               \
    res->children[0]->parent = res;                             \
    res->child_qt = 1;                                          \
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)          \
    next_token();                                               \
    CHECK_TYPE(TYPE_TOCHKA_S_ZAPYATOI)                          \
    next_token();                                               \
    return res;                                                 \
}
GET_SCAN_PRINT(scan(), SCAN)
GET_SCAN_PRINT(print(), PRINT)
#undef GET_SCAN_PRINT
//================================================================
#define GET_WHILE_IF_SAME_PART(Type)                                \
    tree_node* res = tree_node_ctor_i(NULL, Type, 0);               \
    res->children[0] = get_condition();                             \
    res->children[0]->parent = res;                                 \
    CHECK_TYPE_DATA(TYPE_KEY_WORD, KEY_WORD_START_OP_BLOCK)      \
    next_token();                                                   \
    res->children[1] = get_op_block();                              \
    res->children[1]->parent = res;                                 \
    res->child_qt = 2;
  


tree_node* get_while() {
    GET_WHILE_IF_SAME_PART(WHILE)
    return res;
}
//================================================================
tree_node* get_if() {
    GET_WHILE_IF_SAME_PART(IF)
    if(cur_token->type == TYPE_KEY_WORD && cur_token->data.i == KEY_WORD_ELSE) {
        next_token();   //  if or {
        if(cur_token->type == TYPE_KEY_WORD && cur_token->data.i == KEY_WORD_IF) {
            next_token();   // (
            res->children[2] = tree_node_ctor_i(res, CONNECTING_NODE, 0);
            res->children[2]->children[0] = get_if();
            res->children[2]->children[0]->parent =res->children[2];
            res->children[2]->child_qt = 1;
        }
        else {
            CHECK_TYPE_DATA(TYPE_KEY_WORD, KEY_WORD_START_OP_BLOCK)
            next_token();   // to oper block
            res->children[2] = get_op_block();
        }
        res->children[2]->parent = res;
        res->child_qt = 3;
    }
    return res;
}
#undef GET_WHILE_IF_SAME_PART
//================================================================
tree_node* get_condition() {
//    printf("get_condution\n");
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_OPEN)
    next_token();   //left
    tree_node* res;
    tree_node* left;
    tree_node* right;
    if(cur_token->type == TYPE_IDENTIFIER)
        left = get_var();
    else if(cur_token->type == TYPE_CONST)
        left = get_n();
    else
        EXIT_LINE(\n\nERROR at condition, cur_token->str_num)
    
    if(cur_token->type == TYPE_OPERATOR && (cur_token->data.i == OPERATOR_EQUAL ||
    cur_token->data.i == OPERATOR_NOT_EQUAL || cur_token->data.i == OPERATOR_LARGER ||
    cur_token->data.i == OPERATOR_LARGER_EQUAL || cur_token->data.i == OPERATOR_LESS ||
    cur_token->data.i == OPERATOR_LESS_EQUAL)) {
        res = tree_node_ctor_i(NULL, OPERATOR, cur_token->data.i);
        next_token();   //right
    }
    else
        EXIT_LINE(\n\nERROR at condition, cur_token->str_num)

    if(cur_token->type == TYPE_IDENTIFIER)
        right = get_var();
    else if(cur_token->type == TYPE_CONST)
        right = get_n();
    else
        EXIT_LINE(\n\nERROR at condition, cur_token->str_num)

    res->children[0] = left;
    res->children[1] = right;
    res->child_qt = 2;
    left->parent = right->parent = res;
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)
    next_token();       // {
    return res;
}
//===============================================================
tree_node* get_return() {
    tree_node* res = tree_node_ctor_i(NULL, RETURN, 0);
    if(cur_token->type == TYPE_CONST)
        res->children[0] = get_n();
    else {
        CHECK_TYPE(TYPE_IDENTIFIER)
        res->children[0] = get_var();
    }
    res->children[0]->parent = res;
    res->child_qt = 1;
    CHECK_TYPE(TYPE_TOCHKA_S_ZAPYATOI)
    next_token();
    return res;
}
//===============================================================
tree_node* get_func_call() {
    int single_call = 0;
    if(cur_token->prev->type == TYPE_TOCHKA_S_ZAPYATOI ||                               //чет как-то такое себе
        (cur_token->prev->type == TYPE_KEY_WORD && ( cur_token->prev->data.i == KEY_WORD_START_OP_BLOCK ||
        cur_token->prev->data.i == KEY_WORD_END_OP_BLOCK)))
        single_call = 1;
    int func_num = check_function_declaration();
    tree_node* res = tree_node_ctor_ch(NULL, FUNCTION, cur_token->data.ch);
    next_token();       // (
    int arg_qt = 0;
    next_token();       // arg1  or  )
    while(cur_token->type == TYPE_IDENTIFIER || cur_token->type == TYPE_CONST) {
        if(arg_qt == functions[func_num].arg_qt) {
            printf ("\n\n%d \\%d \n\n", arg_qt, functions[func_num].arg_qt);
            EXIT_STR_LINE(\n\nERROR: Too many arguments in function, functions[func_num].name, cur_token->str_num)
        }
        if(cur_token->type == TYPE_IDENTIFIER)
            res->children[res->child_qt] = get_var();
        else
            res->children[res->child_qt] = get_n();
        res->children[(res->child_qt)++]->parent = res;
        arg_qt++;
        if(cur_token->type != TYPE_ZAPYATAYA)
            break;
        next_token();       // arg2
    }
    CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)
    next_token();       // ; if it is a single call
    if(arg_qt < functions[func_num].arg_qt)
        EXIT_STR_LINE(\n\nERROR: Too few arguments in function, functions[func_num].name, cur_token->str_num)
    if(single_call) {
        CHECK_TYPE(TYPE_TOCHKA_S_ZAPYATOI)
        next_token();
    }
    return res;
}
//===============================================================
tree_node* get_var() {
    CHECK_TYPE(TYPE_IDENTIFIER)
    check_variable_declaration();
    tree_node* res = tree_node_ctor_ch(NULL, VARIABLE, cur_token->data.ch);
    next_token();
    return res;
}
//===============================================================
tree_node* get_n() {
    CHECK_TYPE(TYPE_CONST)
    tree_node* res = tree_node_ctor_i(NULL, CONST, cur_token->data.i);
    next_token();
    return res;
}
//===============================================================
tree_node* get_expr() {
    tree_node* res = tree_node_ctor_i(NULL, OPERATOR, OPERATOR_ASSIGNMENT);
    res->children[0] = get_var();       // stop at =
    res->children[0]->parent = res;
    next_token();                       // aftet =
    res->children[1] = get_e();
    res->children[1]->parent = res;
    res->child_qt = 2;

    CHECK_TYPE(TYPE_TOCHKA_S_ZAPYATOI)
    next_token();
    return res;
}
//===============================================================
#define GET_E_T(func, next_func, op1, op2)                                                                                      \
tree_node* func() {                                                                                                             \
    tree_node* val = next_func();                                                                                               \
    tree_node* res = val;                                                                                                       \
    while(cur_token->type == TYPE_OPERATOR && (cur_token->data.i == OPERATOR_##op1 || cur_token->data.i == OPERATOR_##op2)) {   \
        tree_node* new_res = tree_node_ctor_i(NULL, OPERATOR, cur_token->data.i);                                               \
        new_res->children[0] = res;                                                                                             \
        res->parent = new_res;                                                                                                  \
        next_token();                                                                                                           \
        new_res->children[1] = next_func();                                                                                     \
        new_res->children[1]->parent = new_res;                                                                                 \
        new_res->child_qt = 2;                                                                                                  \
        res = new_res;                                                                                                          \
    }                                                                                                                           \
    return res;                                                                                                                 \
}                                                   

GET_E_T(get_e, get_t, ADD, SUB)
GET_E_T(get_t, get_p, MUL, DIV)
#undef GET_E_T
//===============================================================
tree_node* get_p() {
    if(cur_token->type == TYPE_SKOBKI_KRYGLYE && cur_token->data.i == SKOBKA_OPEN) {
        next_token();      //inside of ()
        tree_node* val = get_e();
        CHECK_TYPE_DATA(TYPE_SKOBKI_KRYGLYE, SKOBKA_CLOSE)
        next_token();
        return val;
    }
    else
        return get_expr_element();
}
//===============================================================
tree_node* get_expr_element() {
    if(cur_token->type == TYPE_IDENTIFIER && cur_token->next->type == TYPE_SKOBKI_KRYGLYE && cur_token->next->data.i == SKOBKA_OPEN)
        return get_func_call();
    else if(cur_token->type == TYPE_IDENTIFIER)
        return get_var();
    else if(cur_token->type == TYPE_CONST)
        return get_n();
    else
        EXIT_LINE(\n\nERROR: wrong syntactical constructon , cur_token->str_num)
}



#undef EXIT
#undef EXIT_LINE
#undef EXIT_STR
#undef EXIT_STR_LINE
