void lexical_analyzer(FILE* my_file, struct token_list * list);
int get_file_size(FILE* f);
void delete_comments(char* prog, struct token_list * list);
int get_str_qt(char* text);
void make_str_ptrs(char* text, char** str_ptrs);
void miss_space(char** cur_token_ptr);
void check_identifier(char** cur_token_ptr, int* token_defined, struct token_list * list, int str_num);
void check_number(char** cur_token_ptr, int* token_defined, struct token_list * list, int str_num);

