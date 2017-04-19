#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "enums and defines.cxx"

typedef double TreeNodeType;

class Node {
public:
	Types type;
	TreeNodeType data;
	Node* parent;
	Node * left;
	Node * right;
	Node(const char* str, Node* par);
	~Node();
	int GetPriority();
};

class Differentiator {
private:
	Node* make_tree(FILE* f, Node* CurParent);
	Node* d(Node* CurNode);	//дифференцирует дерево 
	Node* c(Node* CurNode);	//копирует дерево //TODO WTF GAVNO ZHOPA XYUNA NAMES
	Node* simplification(Node* CurNode);	//делает дерево проще
	void save(FILE* f, Node* CurNode);
	void NodeDump(Node* CurNode, FILE* DumpFile);
	int IsLeft(Node* CurNode);
	void visitor(Node* CurNode, Passes pass, void act(Node*));
	Node* d_add(Node* CurNode);
	Node* d_sub(Node* CurNode);
	Node* d_mul(Node* CurNode); //TODO use defines
	Node* d_div(Node* CurNode);
	Node* d_deg(Node* CurNode);
	Node* d_cos(Node* CurNode);
	Node* d_sin(Node* CurNode);
	Node* d_ln(Node* CurNode);
	Node* d_var(Node* CurNode);
	Node* d_const(Node* CurNode);
	Node* simple_mul_zero(Node* CurNode);
	Node* simple_mul_one(Node* CurNode);
	Node* simple_deg_zero(Node* CurNode);
	Node* simple_deg_one(Node* CurNode);
	Node* simple_add_zero(Node* CurNode); //MORE DEFINES
	Node* simple_sub_zero(Node* CurNode);
	void TexPrintf(FILE*f, Node* CurNode);
public:
	Node* tree;
	Node* DifTree;
	Differentiator(FILE* f);
	~Differentiator();
	void differentiator();
	void print(Node* tree);
	void dump();
	void TexDump(FILE* f);
	void ok(Node* CurNode);
};
#include "functions.cxx"
//======================================================================
int main() {
	FILE* in;
	FOPEN(in, "in", "r")
	FILE* TexFile;
	FOPEN(TexFile, "TexDump.tex", "w")
	Differentiator a(in);
	a.differentiator();
	a.dump();
	a.TexDump(TexFile);
	FCLOSE(in)
	FCLOSE(TexFile)
	return 0;
}
//======================================================================
Node :: Node(const char* str, Node* par) {
#define DEF_DATA(Str, Type, Data, function, priority) {	\
	if(!strcmp(str, Str)) {					\
		type = Type;						\
		data = Data;						\
		left = right = NULL;				\
		parent = par;						\
		return;								\
	}										\
}
#include "def_data.cxx"
#undef DEF_DATA
	type = TYPE_CONST;
	data = atof(str);
	left = right = NULL;
	parent = par;
}
//====================================================================== //TODO make two separate files
Node :: ~Node() {
	if(left)
		delete left;
	if(right)
		delete right;
	parent = right = left = NULL;
}
//======================================================================
Differentiator ::Differentiator(FILE* f) {
	tree = make_tree(f, NULL);
}
Differentiator :: ~Differentiator() {
	FILE* out; //TODO output file from main
	FOPEN(out, "Otvet", "w")
	save(out, DifTree);
	FCLOSE(out)
	delete tree;
	delete DifTree;
	tree = NULL;
	DifTree = NULL;
}
//======================================================================
#define CHECK_CHAR(a) {						\
	CurChar = fgetc(f);						\
	if(CurChar != a) {						\
		printf("Wrong input construction");	\
		exit(1);							\
	}										\
}
Node* Differentiator ::  make_tree(FILE* f, Node* CurFather) {
	char data[MAX_STR_LEN] = {};
	assert(MAX_STR_LEN == 11);
	char CurChar = '\0';
	CHECK_CHAR('(')
	fscanf(f, "%10[^()]", data);
	Node* CurNode = new Node(data, CurFather);
	
	CurChar = fgetc(f);
	ungetc(CurChar, f);
	if(CurChar == '(')
		CurNode->left = make_tree(f, CurNode);
		
	CurChar = fgetc(f);
	ungetc(CurChar, f);
	if(CurChar == '(')
		CurNode->right = make_tree(f, CurNode);
		
	CHECK_CHAR(')')
	return CurNode;
}
#undef CHECK_CHAR
//======================================================================
void Differentiator :: save(FILE* f, Node* CurNode) {
	fprintf(f, "(");
#define DEF_DATA(Str, Type, Data, function, priority) {		\
	if(CurNode->type == Type && Type != TYPE_CONST && CurNode->data == Data) {	\
		fprintf(f, Str);												\
		return;	\
	}	\
}
#include "def_data.cxx"
#undef DEF_DATA
	if(CurNode->type == TYPE_CONST)
		fprintf(f, "%lg", CurNode->data);
	if(CurNode->left)
		save(f, CurNode->left);
	if(CurNode->right)
		save(f, CurNode->right);
	fprintf(f, ")");
}
//======================================================================
Node* Differentiator :: d(Node* CurNode) {
	Node* DifNode = NULL;
#define DEF_DATA(Str, Type, Data, function, priority) {														\
	if((CurNode->type == Type && Type != TYPE_CONST && CurNode->data == Data) || (CurNode->type == Type && Type == TYPE_CONST))	\
		DifNode = function(CurNode);																\
}
#include "def_data.cxx"
#undef DEF_DATA
	return DifNode;
}
//======================================================================
Node* Differentiator :: simplification(Node* CurNode) {
	if(CurNode->left)
		CurNode->left = simplification(CurNode->left);
	if(CurNode->right)
		CurNode->right = simplification(CurNode->right);
#define SIMPLE(operation, num, function) {																\
	if(CurNode->type == TYPE_ACTION && CurNode->data == operation &&									\
		(((CurNode->left) && CurNode->left->type == TYPE_CONST && CurNode->left->data == num)			\
		|| ((CurNode->right) && CurNode->right->type == TYPE_CONST && CurNode->right->data == num))) {	\
		CurNode = function(CurNode);																				\
		return CurNode;																							\
	}																									\
}
	SIMPLE(FUNC_ADD, 0, simple_add_zero)
	SIMPLE(FUNC_SUB, 0, simple_sub_zero)
	SIMPLE(FUNC_MUL, 1, simple_mul_one)
	SIMPLE(FUNC_MUL, 0, simple_mul_zero)
	SIMPLE(FUNC_DEG, 1, simple_deg_one)
	SIMPLE(FUNC_DEG, 0, simple_deg_zero)
#undef SIMPLE
	return CurNode;
}
//======================================================================
void Differentiator :: dump() {
	FILE* DumpFile;
	FOPEN(DumpFile, "dumpfile.gv", "w")
	fprintf(DumpFile,"digraph graf{\n"); 
	NodeDump(DifTree,DumpFile); 
	fprintf(DumpFile,"}"); 
	FCLOSE(DumpFile); 
	system ("dot dumpfile.gv -Tpng -o dumpfile.png");
	system ("xdot dumpfile.gv");
}
void Differentiator :: NodeDump(Node* CurNode, FILE* DumpFile) { 
	fprintf (DumpFile, "treeNode_%p [label=\"\\l", CurNode);
#define DEF_DATA(Str, Type, Data, Function, priority) {								\
	if(CurNode->type == Type && Type != TYPE_CONST && CurNode->data == Data)		\
		fprintf(DumpFile, Str);												\
}
#include "def_data.cxx"
#undef DEF_DATA
	if(CurNode->type == TYPE_CONST)
		fprintf(DumpFile, "%lg", CurNode->data);
		fprintf(DumpFile, "\"]\n");
	if(CurNode->left) {
		fprintf (DumpFile, "treeNode_%p->treeNode_%p\n", CurNode, CurNode->left);
		NodeDump (CurNode->left, DumpFile); 
	}
	if(CurNode->right) {
		fprintf (DumpFile, "treeNode_%p->treeNode_%p\n", CurNode, CurNode->right);
		NodeDump (CurNode->right, DumpFile); 
	}
}
//======================================================================
void Differentiator :: differentiator() {
	DifTree = d(tree);
	DifTree = simplification(DifTree);
}
//======================================================================
int Differentiator :: IsLeft(Node* CurNode) {
	if(!(CurNode->parent))
		return -1;
	if(CurNode->parent->left == CurNode)
		return 1;
	else
		return 0;
}
//======================================================================
void Differentiator :: ok(Node* CurNode) {
	Tree_Status TreeStat = STATUS_NOT_OK;
	if
	(
	(!CurNode) || 
	((CurNode->left) && CurNode->left->parent != CurNode) ||
	((CurNode->right) && CurNode->right->parent != CurNode) ||
	(CurNode->left == CurNode->right)) {
		TreeStat = STATUS_NOT_OK; 
	} else {
		TreeStat = STATUS_OK;
	}
	if(CurNode->left)
		ok(CurNode->left);
	if(CurNode->right)
		ok(CurNode->right);
}
//======================================================================
void visitor(Node* CurNode, Passes pass, void act(Node*)) {
	if(!CurNode)
		return;
	switch(pass) {
		case PASS_PREFIX:
			act(CurNode);
			visitor(CurNode->left, pass, act);
			visitor(CurNode->right, pass, act);
			break;
		case PASS_INFIX:
			visitor(CurNode->left, pass, act);
			act(CurNode);
			visitor(CurNode->right, pass, act);
			break;
		case PASS_POSTFIX:
			visitor(CurNode->left, pass, act);
			visitor(CurNode->right, pass, act);
			act(CurNode);
			break;
		}
}
//======================================================================
void Differentiator :: TexDump(FILE* f) {
	printf("XUUUI");
	fprintf(f, "\\documentclass{article}\n"
				"\\usepackage[a4paper,margin=6mm]{geometry}\n"
				"\\usepackage{amsmath}\n"
				"\\usepackage[utf8]{inputenc}\n"
				"\\usepackage[T2A]{fontenc}\n"
				"\\usepackage[russian]{babel}\n"
				"\\usepackage{hyperref}\n"
				"%%\\title{\\LaTeX\\  Derivation}\n"
				"%%\\author{asjklksejfRRRR}\n"
				"\\begin{document}\n"
				"%%\\maketitle\n"
				"\\fontsize{8}{8pt}\\selectfont\n"
				"\\begin{equation}\n");
	TexPrintf(f,DifTree);
	fprintf(f, "\\end{equation}\n"
				"\\end{document}\n");
}
//======================================================================
void Differentiator :: TexPrintf(FILE* f, Node* CurNode) {
	if(!CurNode)
		return;
	int scobki = ((CurNode->parent) && CurNode->GetPriority() < CurNode->parent->GetPriority());
	if(scobki)
		fprintf(f, "\\left(");
	switch(CurNode->type) {
		case TYPE_ACTION:
			switch((int)CurNode->data) {
				case FUNC_ADD:
					TexPrintf(f, CurNode->left);
					fprintf(f, "+");
					TexPrintf(f, CurNode->right);
					break;
				case FUNC_SUB:
					TexPrintf(f, CurNode->left);
					fprintf(f, "-");
					TexPrintf(f, CurNode->right);
					break;
				case FUNC_MUL:
					TexPrintf(f, CurNode->left);
					fprintf(f, "*");
					TexPrintf(f, CurNode->right);
				case FUNC_DIV:
					fprintf(f, "\\frac{");
					TexPrintf(f, CurNode->left);
					fprintf(f, "}{");
					TexPrintf(f, CurNode->right);
					fprintf(f, "}");
					break;
				case FUNC_DEG:
					fprintf(f, "{");
					TexPrintf(f, CurNode->left);
					fprintf(f, "}^{");
					TexPrintf(f, CurNode->right);
					fprintf(f, "}");
					break;
				case FUNC_COS:
					fprintf(f, "\\cos{");
					TexPrintf(f, CurNode->left);
					fprintf(f, "}");
					break;
				case FUNC_SIN:
					fprintf(f, "\\sin{");
					TexPrintf(f, CurNode->left);
					fprintf(f, "}");
					break;
				case FUNC_LN:
					fprintf(f, "\\log{");
					TexPrintf(f, CurNode->left);
					fprintf(f, "}");
					break;
			}
			break;
		case TYPE_CONST:
			fprintf(f, "%lg", CurNode->data);
			break;
		case TYPE_VAR:
			fprintf(f, "x");
			break;
	}
	if(scobki)
		fprintf(f, "\\right)");
}
//======================================================================
int Node :: GetPriority() {
#define DEF_DATA(Str, Type, Data, Function, Priority) {								\
	if((type == Type && Type != TYPE_CONST && data == Data) || (type == Type && Type == TYPE_CONST))	\
		return Priority;														\
}
#include "def_data.cxx"
#undef DEF_DATA
	return 0;
}












