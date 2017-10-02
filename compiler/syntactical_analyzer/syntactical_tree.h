struct tree_node {
enum NODE_TYPES type;
union union_type data;
struct tree_node * parent;
struct tree_node * children[MAX_CHILDREN_QT];
int child_qt;
};

typedef struct tree_node tree_node;
//======================================================================
tree_node* tree_node_ctor_i(tree_node* parent, NODE_TYPES type, int Data);
tree_node* tree_node_ctor_ch(tree_node* parent, NODE_TYPES type, char* Data);

void tree_dtor(tree_node* cur_node);
void tree_dump(tree_node* tree);
void tree_node_dump(tree_node* cur_node, FILE* dot_dump_file);

