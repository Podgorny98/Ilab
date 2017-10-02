struct token_node {
    int type;
    union union_type data;
    int str_num;
    struct token_node * next;
    struct token_node * prev;
};

struct token_list {
    struct token_node * first;
    struct token_node * last;
};
typedef struct token_list token_list;
typedef struct token_node token_node;
//========================================================================
struct token_list * token_ctor();
void token_detor(struct token_list * list);
void token_push_i(struct token_list * list, enum TOKEN_TYPES Type, int Data, int Str_Num);
void token_push_ch(struct token_list * list, enum TOKEN_TYPES Type, char* Data, int len, int Str_Num);
void token_list_dump(struct token_list  * list);
void token_node_dump(struct token_node *  cur_node, FILE* dump_file);
