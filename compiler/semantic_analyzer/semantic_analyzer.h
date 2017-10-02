struct func_description {
    char* name;
    int arg_qt;
    int var_qt;
    char* args[MAX_ARGS_QT];
    char* vars[MAX_NEW_VARS_QT];
};

void semantic_analyzer(tree_node* syntactical_tree);
int get_arg_var_offset(struct func_description s, char* var_name);
struct func_description func_description_ctor(tree_node* func_node);

void translate_function(tree_node* cur_node);
void translate_operator(tree_node* cur_node, struct func_description cur_function);
void translate_scan(tree_node* cur_node, struct func_description cur_function); 
void translate_print(tree_node* cur_node, struct func_description cur_function); 

void translate_while(tree_node* cur_node, struct func_description cur_function);
void translate_if(tree_node* cur_node, struct func_description cur_function);
void condition_jump(tree_node* condition_node, struct func_description cur_function);

void translate_return(tree_node* cur_node, struct func_description cur_function);
void translate_func_call(tree_node* cur_node, struct func_description cur_function);
void translate_func_call_args(tree_node* cur_node, struct func_description cur_function);

void translate_expr(tree_node* cur_node, struct func_description cur_function);
void translate_math_op(tree_node* cur_node, struct func_description cur_function);
char* get_math_com_name(int com_num);




