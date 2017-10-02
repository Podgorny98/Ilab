#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../enums"
#include "../lexical_analyzer/token_list.h"
#include "syntactical_tree.h"
//======================================================================
tree_node* tree_node_ctor_i(tree_node* Parent, NODE_TYPES Type, int Data) {
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    node->type = Type;
    node->data.i = Data;
    node->parent = Parent;
    for(int i = 0; i < MAX_CHILDREN_QT; i++)
        node->children[i] = NULL;
    node->child_qt = 0;
    return node;
}
//======================================================================
tree_node* tree_node_ctor_ch(tree_node* Parent, NODE_TYPES Type, char* Data) {
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    node->type = Type;
    node->data.ch = (char*)malloc((strlen(Data) + 1) * sizeof(char));
    strcpy(node->data.ch, Data);
    node->parent = Parent;
    for(int i = 0; i < MAX_CHILDREN_QT; i++)
        node->children[i] = NULL;
    node->child_qt = 0;
    return node;
}
//======================================================================
void tree_dtor(tree_node* cur_node) {
    for(int i = 0; i < cur_node->child_qt; i++) { 
        if(cur_node->children[i]) {
            tree_dtor(cur_node->children[i]);
            cur_node->children[i] = NULL;
        }
    }
    cur_node->child_qt = 0;
    if(cur_node->type == FUNCTION || cur_node->type == VARIABLE) {
        free(cur_node->data.ch);
        cur_node->data.ch = NULL;
    }
    free(cur_node);
    cur_node = NULL;
}
//======================================================================
void tree_dump(tree_node* tree) {
FILE* dot_dump_file;
	dot_dump_file = fopen("dumpfile.gv", "w");
	fprintf(dot_dump_file,"digraph graf{\n"); 
	tree_node_dump(tree ,dot_dump_file); 
	fprintf(dot_dump_file,"}"); 
	fclose(dot_dump_file);
	system ("dot dumpfile.gv -Tpng -o dumpfile.png");
	system ("xdot dumpfile.gv");
}
//======================================================================
void tree_node_dump(tree_node* cur_node, FILE* dot_dump_file) {
    fprintf (dot_dump_file, "treeNode_%p [label=\"\\l", cur_node);
#define DEF_PRINTF_TREE_NODE(Type, Data, Str)  			            \
	if(cur_node->type == Type && cur_node->data.i == Data)		    \
		fprintf(dot_dump_file, Str);								\
 
#include "def_printf_tree_node"
#undef DEF_PRINTF_TREE_NODE
    else if(cur_node->type == FUNCTION)
        fprintf(dot_dump_file, "FUNCTION\\l %s()", cur_node->data.ch);
	else if(cur_node->type == VARIABLE)
        fprintf(dot_dump_file, "%s", cur_node->data.ch);
    else if(cur_node->type == CONST)
		fprintf(dot_dump_file, "%d", cur_node->data.i);
	fprintf(dot_dump_file, "\"]\n");
	for(int i = 0; i < cur_node->child_qt; i++) {
		if(cur_node->children[i]) {
			fprintf (dot_dump_file, "treeNode_%p->treeNode_%p\n", cur_node, cur_node->children[i]);
			tree_node_dump(cur_node->children[i], dot_dump_file); 
		}
	}
}











