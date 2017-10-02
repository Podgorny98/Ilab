void next_token();
tree_node* syntactical_analyzer(struct token_list * list);
tree_node* get_programm();
tree_node* get_function();

void check_only_function_declaration();
void check_only_variable_declaration();
int check_function_declaration();
int check_variable_declaration();

tree_node* get_args();
tree_node* get_new_vars();
tree_node* get_op_block();
tree_node* get_operator();
tree_node* get_scan(); 
tree_node* get_print(); 
tree_node* get_while();
tree_node* get_if();
tree_node* get_condition();
tree_node* get_return();
tree_node* get_func_call();
tree_node* get_var();
tree_node* get_n();
tree_node* get_expr();

tree_node* get_e();
tree_node* get_t();
tree_node* get_p();
tree_node* get_expr_element();



























