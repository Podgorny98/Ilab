#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "enums and defines.cxx"
typedef double TreeNodeType;
#include "class Node.cxx"
#define left children[0]
#define right children[1]

class Differentiator {
private:
	Node* make_tree(FILE* f, Node* CurParent);
	Node* dif(Node* CurNode);	//дифференцирует дерево 
	Node* copy(Node* CurNode);	//копирует дерево
	Node* simplification(Node* CurNode);	//делает дерево проще
	void GeneralSimplification(Node* CurNode);
	void ToBinarnoe(Node* CurNode);
	void ToNarnoe(Node* CurNode);
	void save(FILE* f, Node* CurNode);
	void NodeDump(Node* CurNode, FILE* DumpFile);
	int IsLeft(Node* CurNode);
	void visitor(Node* CurNode, Passes pass, void act(Node*));
	Node* PrivedeniePodobnyh(Node* CurNode);
	Node* SvertkaConstant(Node* CurNode);
	int compare(Node* node1, Node* node2);
	double CheckSimilarSummands(Node* n1, Node* n2);
	double CheckSimilarMultipliers(Node* n1, Node* n2);
	void PrivedeniePodobnyhSlagaemyh(Node* CurNode);
	void PrivedeniePodobnyhMnojitelei(Node* CurNode);
	Node* AddMulWithOneSon(Node* CurNode);	//упрощает дерево если у + или * один ребёнок
	int WhatSon(Node* CurNode);	//определяе каким ребёнком является узел у родителя
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
	Node* simple_div_zero(Node* CurNode);
	Node* simple_div_one(Node* CurNode);
	void TexPrintf(FILE*f, Node* CurNode);
public:
	Node* tree;
	Node* DifTree;
	Differentiator(FILE* f);
	~Differentiator();
	FILE* TexFile;
	void BeginDump();
	void AddDump();
	void EndDump();
	void differentiator();
	void print(Node* tree);
	void dump();
	void TexDump();
	void ok(Node* CurNode);
};
#include "functions.cxx"
#include "dump functions.cxx"
//======================================================================
int main() {
	{
	FILE* in;
	FOPEN(in, "in", "r")
	Differentiator a(in);
	//a.dump();
	a.differentiator();
	
	//a.TexDump();
	FCLOSE(in)
	}
	{
	FILE* in;
	FOPEN(in, "Otvet", "r")
	
	Differentiator a(in);
	a.differentiator();
	a.dump();
	a.TexDump();
	FCLOSE(in)
	return 0;
	}
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
#define DEF_DATA(Str, Type, Data, Childqt, Function, priority) {				\
	if(CurNode->type == Type && Type != TYPE_CONST && CurNode->data == Data) {	\
		fprintf(f, Str);														\
	}																			\
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
Node* Differentiator :: dif(Node* CurNode) {
	Node* DifNode = NULL;
#define DEF_DATA(Str, Type, Data, Childqt, Function, priority) {														\
	if((CurNode->type == Type && Type != TYPE_CONST && CurNode->data == Data) || (CurNode->type == Type && Type == TYPE_CONST))	\
		DifNode = Function(CurNode);																\
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
		CurNode = function(CurNode);																	\
	}																									\
}
	SIMPLE(FUNC_ADD, 0, simple_add_zero)
	SIMPLE(FUNC_SUB, 0, simple_sub_zero)
	SIMPLE(FUNC_MUL, 1, simple_mul_one)
	SIMPLE(FUNC_MUL, 0, simple_mul_zero)
	SIMPLE(FUNC_DEG, 1, simple_deg_one)
	SIMPLE(FUNC_DEG, 0, simple_deg_zero)
	SIMPLE(FUNC_DIV, 0, simple_div_zero)
	SIMPLE(FUNC_DIV, 1, simple_div_one)
#undef SIMPLE
	GeneralSimplification(CurNode);
	return CurNode;
}
//======================================================================
void Differentiator :: differentiator() {
	ToNarnoe(tree);
	tree = SvertkaConstant(tree);
	ToBinarnoe(tree);
	DifTree = dif(tree);
	BeginDump();
	AddDump();
	for(int i = 0; i < 36; i++) {
		DifTree = simplification(DifTree);
		ToNarnoe(DifTree);
		DifTree = SvertkaConstant(DifTree);
		DifTree = PrivedeniePodobnyh(DifTree);
		ToBinarnoe(DifTree);
		AddDump();
	}
	EndDump();
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
void Differentiator :: ToNarnoe(Node* CurNode) {
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i])
			ToNarnoe(CurNode->children[i]);
	}
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_SUB) {
		CurNode->data = FUNC_ADD;
		Node* Right = new Node("*", CurNode);
		Right->left = new Node("-1", Right);
		Right->right = CurNode->right;
		CurNode->right->parent = Right;
		CurNode->right = Right;
		Right->ChildQt = 2;
	}
	else if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_DIV) {
		Node* Right = CurNode->right;
		CurNode->data = FUNC_MUL;
		CurNode->right = new Node("/", CurNode);
		CurNode->right->left = new Node("1", CurNode->right);
		CurNode->right->right = Right;
		Right->parent = CurNode->right;
	}
#define LINK_ADD_MUL_WITH_ONE_NODE(func) {														\
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_##func) {							\
		for(int i = 0; i < CurNode->ChildQt; i++) {												\
			Node* CurChild = CurNode->children[i];												\
			if((CurChild) && CurChild->type == TYPE_ACTION && CurChild->data == FUNC_##func) {	\
				CurNode->children[i] = copy(CurChild->children[0]);								\
				CurNode->children[i]->parent = CurNode;											\
				for(int j = 1; j < CurChild->ChildQt; j++) {									\
					CurNode->children[(CurNode->ChildQt)] = copy(CurChild->children[j]);			\
					CurNode->children[(CurNode->ChildQt)++]->parent = CurNode;					\
				}																				\
			delete CurChild;																	\
			}																					\
		}																						\
	return;																						\
	}																							\
}
	LINK_ADD_MUL_WITH_ONE_NODE(ADD)
	LINK_ADD_MUL_WITH_ONE_NODE(MUL)
}
#undef LINK_ADD_MUL_WITH_ONE_NODE
//======================================================================
void Differentiator :: ToBinarnoe(Node* CurNode) {
#define ADD_MUL_TO_BINAR(func, act) {																\
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_##func && CurNode->ChildQt > 2) {	\
		Node* Left = new Node(act, CurNode);											\
		for(int i = 0; i < CurNode->ChildQt - 1; i++) {									\
			CurNode->children[i]->parent = Left;										\
			Left->children[i] = CurNode->children[i];									\
			CurNode->children[i] = NULL;												\
		}																				\
		Left->ChildQt = CurNode->ChildQt - 1;											\
		CurNode->right = CurNode->children[CurNode->ChildQt -1];						\
		CurNode->children[CurNode->ChildQt -1] = NULL;									\
		CurNode->left = Left;															\
		CurNode->ChildQt = 2;															\
	}																					\
}
	ADD_MUL_TO_BINAR(ADD, "+")
	ADD_MUL_TO_BINAR(MUL, "*")
	if(CurNode->left)
		ToBinarnoe(CurNode->left);
	if(CurNode->right)
		ToBinarnoe(CurNode->right);
}
Node* Differentiator :: SvertkaConstant(Node*CurNode) {
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i])
			SvertkaConstant(CurNode->children[i]);
	}
#define SVERTKA_ADD_MUL(func, act) {													\
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_##func) {					\
		for(int i = 0; i < CurNode->ChildQt; i++) {										\
			if(CurNode->children[i]->type == TYPE_CONST) {								\
				for(int j = i + 1; j < CurNode->ChildQt; j++) {							\
					if(CurNode->children[j]->type == TYPE_CONST) {						\
						CurNode->children[i]->data act CurNode->children[j]->data;		\
						delete CurNode->children[j];									\
						CurNode->children[j] = CurNode->children[CurNode->ChildQt - 1];	\
						CurNode->children[CurNode->ChildQt - 1] = NULL;					\
						(CurNode->ChildQt)--;											\
						j--;															\
					}																	\
				}																		\
				break;																	\
			}																			\
		}																				\
	}																					\
}
	SVERTKA_ADD_MUL(ADD, +=)
	SVERTKA_ADD_MUL(MUL, *=)
	CurNode = AddMulWithOneSon(CurNode);
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_DIV &&
	CurNode->left->type == TYPE_CONST && CurNode->right->type == TYPE_CONST) {
		CurNode->type = TYPE_CONST;
		CurNode->data = CurNode->left->data / CurNode->right->data;
		delete CurNode->left;
		delete CurNode->right;
		CurNode->left = CurNode->right = NULL;
	}
	return CurNode;
}
//======================================================================
int Differentiator :: compare(Node* n1, Node* n2) {
	if(n1->type == n2->type && n1->data == n2->data && n1->ChildQt == n2->ChildQt) {
		if(n1->ChildQt > 0) {
			for(int i = 0; i < n1->ChildQt; i++) {
				if(!compare(n1->children[i], n2->children[i]))
					return 0;
			}
		}
		return 1;
	}
	return 0;
}
//======================================================================
double Differentiator :: CheckSimilarMultipliers(Node* n1, Node* n2) {
	if(n1->type == TYPE_ACTION && n1->data == FUNC_DEG && n1->right->type == TYPE_CONST && compare(n1->left, n2))
		return n1->right->data + 1;
	if(n2->type == TYPE_ACTION && n2->data == FUNC_DEG && n2->right->type == TYPE_CONST && compare(n2->left, n1))
		return n2->right->data + 1;
	if(n1->type == TYPE_ACTION && n1->data == FUNC_DEG && n1->right->type == TYPE_CONST &&
		n2->type == TYPE_ACTION && n2->data == FUNC_DEG && n2->right->type == TYPE_CONST && compare(n2->left, n1->left))
		return n1->right->data + n2->right->data;
	if(compare(n1, n2))
		return 2;
	return 0;
}
//======================================================================
double Differentiator :: CheckSimilarSummands(Node* n1, Node* n2) {
	if(n1->type == TYPE_ACTION && n1->data == FUNC_MUL && n1->ChildQt == 2 && n1->left->type == TYPE_CONST) {
		if(compare(n1->right, n2))
			return n1->left->data + 1;
		if(n2->type == TYPE_ACTION && n2->data == FUNC_MUL && n2->ChildQt == 2) {
			if(n2->left->type == TYPE_CONST && compare(n1->right, n2->right))
				return n1->left->data + n2->left->data;
			if(n2->right->type == TYPE_CONST && compare(n1->right, n2->left))
				return n1->left->data + n2->right->data;
		}
	}
	if(n1->type == TYPE_ACTION && n1->data == FUNC_MUL && n1->ChildQt == 2 && n1->right->type == TYPE_CONST) {
		if(compare(n1->left, n2))
			return n1->right->data + 1;
		if(n2->type == TYPE_ACTION && n2->data == FUNC_MUL && n2->ChildQt == 2) {
			if(n2->left->type == TYPE_CONST && compare(n1->left, n2->right))
				return n1->right->data + n2->left->data;
			if(n2->right->type == TYPE_CONST && compare(n1->left, n2->left))
				return n1->right->data + n2->right->data;
		}
	}
	if(n2->type == TYPE_ACTION && n2->data == FUNC_MUL && n2->ChildQt == 2) {
		if(n2->left->type == TYPE_CONST && compare(n1, n2->right))
			return 1 + n2->left->data;
		if(n2->right->type == TYPE_CONST && compare(n1, n2->left))
			return 1 + n2->right->data;
	}
	if(compare(n1, n2))
		return 2;
	return 0;
}
//======================================================================
Node* Differentiator :: PrivedeniePodobnyh(Node*CurNode) {
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i])
			CurNode->children[i] = PrivedeniePodobnyh(CurNode->children[i]);
	}
	PrivedeniePodobnyhMnojitelei(CurNode);
	PrivedeniePodobnyhSlagaemyh(CurNode);
	CurNode = AddMulWithOneSon(CurNode);
	return CurNode;
}
//======================================================================
#define NEWCHILD(str) {												\
	Node* NewChild = new Node(str, CurNode);						\
	NewChild->ChildQt = 2;											\
	NewChild->right = new Node("1", NewChild);						\
	NewChild->right->data = deg;									\
	NewChild->left = CurNode->children[i];							\
	CurNode->children[i]->parent = NewChild;						\
	CurNode->children[i] = NewChild;								\
	delete CurNode->children[j];									\
	CurNode->children[j] = CurNode->children[CurNode->ChildQt - 1];	\
	CurNode->children[--(CurNode->ChildQt)] = NULL;					\
}
#define PRIVEDENIE(child, func)																			\
	if(CurNode->children[i]->type == TYPE_ACTION && CurNode->children[i]->data == FUNC_##func && 	\
	CurNode->children[i]->child->type == TYPE_CONST) {											\
		CurNode->children[i]->child->data = deg;												\
		delete CurNode->children[j];															\
		CurNode->children[j] = CurNode->children[CurNode->ChildQt - 1];							\
		CurNode->children[--(CurNode->ChildQt)] = NULL;											\
	}																							
//======================================================================
void Differentiator :: PrivedeniePodobnyhMnojitelei(Node* CurNode) {
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_MUL) {
		for(int i = 0; i < CurNode->ChildQt; i++) {
			for(int j = i + 1; j < CurNode->ChildQt; j++) {
				double deg = CheckSimilarMultipliers(CurNode->children[i], CurNode->children[j]);
				if(deg) {
					PRIVEDENIE(right, DEG)
					else {
						NEWCHILD("^")
					}
				j--;
				}
			}
		}
	}
}
//======================================================================
void Differentiator :: PrivedeniePodobnyhSlagaemyh(Node* CurNode) {
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_ADD) {
		for(int i = 0; i < CurNode->ChildQt; i++) {
			for(int j = i + 1; j < CurNode->ChildQt; j++) {
				double deg = CheckSimilarSummands(CurNode->children[i], CurNode->children[j]);
				if(deg) {
					PRIVEDENIE(left, MUL)
					else PRIVEDENIE(right, MUL)
					else {
						NEWCHILD("*")
					}
				j--;
				}
			}
		}
	}
}
#undef PRIVEDENIE
#undef NEWCHILD
//======================================================================
Node* Differentiator :: AddMulWithOneSon(Node* CurNode) {
	if(CurNode->type == TYPE_ACTION && (CurNode->data == FUNC_ADD || CurNode->data == FUNC_MUL) && CurNode->ChildQt == 1) {
		if(CurNode->children[0]->ChildQt == 0) {
			CurNode->type = CurNode->children[0]->type;
			CurNode->data = CurNode->children[0]->data;
			CurNode->ChildQt = 0;
			delete CurNode->children[0];
			CurNode->children[0] = NULL;
		}
		else {
			Node* cur = CurNode;
			CurNode = copy(CurNode->children[0]);
			if(cur->parent) {
				CurNode->parent = cur->parent;
				cur->parent->children[WhatSon(cur)] = CurNode;
			}
			delete cur;
		}
	}
	return CurNode;
}
//======================================================================
int Differentiator :: WhatSon(Node* CurNode) {
	for(int i = 0; i < CurNode->parent->ChildQt; i++) {
		if(CurNode->parent->children[i] == CurNode)
			return i;
	}
	return -1;
}
