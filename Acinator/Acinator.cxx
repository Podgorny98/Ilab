#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "defines.cxx"
//=====================================================================
class Node {
public:
	char data[MAX_STR_LEN];
	Node * left;
	Node * right;
	Node * father;
	Node(Node* p, char* q);
	~Node();
	void print();
};

class Akinator {
private:
	TREE_ERRS TreeStatus;
	Node* make_tree(FILE* f, Node* CurFather);
	void ChangeTree(Node* CurNode);
	void NodeDump(Node* CurNode, FILE* DumpFile);
public:
	Node* GodFather;
	Akinator(FILE* f);
	~Akinator();
	void save(FILE* f, Node* CurNode);
	void print(Node* tree);
	void play(Node* CurNode);
	void dump();
	void ok(Node* CurNode);
	void menu();
	int IsLeft(Node* CurNode);
};

//======================================================================
int main()
{
	FILE* in;
	
	FOPEN(in, "Teachers", "r")
	Akinator a(in);
	a.menu();
	FCLOSE(in)
	return 0;
}
//======================================================================
void Node :: print() {
	if(!this)
		return;
	printf("%s\n", data);
	left->print();
	right->print();
}
//=================================s=====================================
Node :: Node(Node* p, char* q) {
	strncpy(data, q, MAX_STR_LEN-1);
	left = NULL;
	right = NULL;
	father = p;
}
//======================================================================
Node :: ~Node() {
	if(left)
		delete left;
	if(right)
		delete right;
	right = father = left = NULL;
}
//======================================================================
Akinator :: ~Akinator() {
	FILE* out;
	FOPEN(out, "New", "w")
	save(out, GodFather);
	FCLOSE(out)
	delete GodFather;
	GodFather = NULL;
}
//======================================================================
Akinator :: Akinator(FILE* f) {
	TreeStatus = ERR_NO;
	GodFather = make_tree(f, NULL);
}
//=====================================================================
Node* Akinator :: make_tree(FILE *f, Node* CurFather) {
	char data[MAX_STR_LEN] = {};
	assert(MAX_STR_LEN == 256);
	char CurChar = 0;
	CHECK_CHAR('(')
	fscanf(f, "%255[^()]", data);
	Node* CurNode = new Node(CurFather, data);
	fscanf(f, "%c", &CurChar);
	if(CurChar == ')')
		return CurNode;
	fseek(f, -1, SEEK_CUR);
	CurNode->left = make_tree(f, CurNode);
	CurNode->right = make_tree(f, CurNode);
	CHECK_CHAR(')')
	return CurNode;
}
//======================================================================
void Akinator :: save(FILE* f, Node* CurNode) {
	fprintf(f, "(%s", CurNode->data);
	if(CurNode->left)
		save(f, CurNode->left);
	if(CurNode->right)
		save(f, CurNode->right);
	fprintf(f, ")");
}
//======================================================================
void Akinator :: play(Node* CurNode) {
	char answer = '\0';
	printf("%s\n", CurNode->data);
	while(!scanf(" %c%*[^\n]", &answer));
	if(answer == 'y') {
		if(CurNode->right) {
			play(CurNode->right);
		}
		else {
			printf("GAME OVER\n");
			return;
		}
	}
	else {
		if(CurNode->left)
			play(CurNode->left);
		else 
			ChangeTree(CurNode);
	}
}
//======================================================================
void Akinator :: ChangeTree(Node* CurNode) {
	char NewQuestion[MAX_STR_LEN] = {};
	char NewAnswer[MAX_STR_LEN] = {};
	printf("\nPlease, enter distinction of your variant from our in less than 255 symbols\n");
	scanf(" %254[^\n]", NewQuestion);
	strcat(NewQuestion, "?");
	printf("\nPlease, enter your variant in next form: Is it _________?\n");
	scanf(" %254[^\n]", NewAnswer);
	Node* QuestNode  = new Node(CurNode->father, NewQuestion);
	Node* YesNode    = new Node(QuestNode,       NewAnswer);
	int isLeft = IsLeft(CurNode);
	
	if(isLeft == 1)
		CurNode->father->left = QuestNode;
	else if (isLeft == 0)
		CurNode->father->right = QuestNode;
	else if (isLeft != -1)
	{
		printf("\nWTF?\n");
		exit(1);
	}
	QuestNode->left = CurNode;
	QuestNode->right = YesNode;
	CurNode->father = QuestNode;

	printf("\nThank you, vsego horoshego\n");
}
//======================================================================
void Akinator :: dump() {
	FILE* DumpFile;
	FOPEN(DumpFile, "dumpfile.gv", "w")
	fprintf(DumpFile,"digraph graf{\n"); 
	NodeDump(GodFather,DumpFile); 
	fprintf(DumpFile,"}"); 
	FCLOSE(DumpFile); 
	system ("dot dumpfile.gv -Tpng -o dumpfile.png");
	system ("xdot dumpfile.gv");
}
void Akinator :: NodeDump(Node* CurNode, FILE* DumpFile) { 
	fprintf (DumpFile, "treeNode_%p [label=\""
						"[%p]\\l"
						"left [%p]\\l"
						"right [%p]\\l"
						"father [%p]\\l"
						"data[%p] = %s"
						"\"]\n",
	CurNode, CurNode, CurNode->left, CurNode->right, CurNode->father, CurNode->data, CurNode->data);
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
void Akinator :: ok(Node* CurNode) {
#define PUTERR(condition, name)				\
	if(condition) TreeStatus = ERR_##name;
	PUTERR(!CurNode, NULL_PTR)
	PUTERR(CurNode->father != NULL && CurNode->father->left != CurNode && CurNode->father->right != CurNode, FATHER_PTR)
	PUTERR(CurNode->left != NULL && CurNode->left->father != CurNode, LEFT_PTR)
	PUTERR(CurNode->right != NULL && CurNode->right->father != CurNode, RIGHT_PTR)
	PUTERR(!(CurNode->data), WRONG_DATA)
	if(CurNode->left)
		ok(CurNode->left);
	if(CurNode->right)
		ok(CurNode->right);
}
void Akinator :: menu() {
	while(1) {
		printf("Press:\n");
		printf("[1] - play\n");
		printf("[2] - dump\n");
		printf("[3] - exit\n");
		int ans = 0;
		bool condition = true;
		while(condition) {
			scanf(" %d", &ans);
			switch(ans) {
				case 1:
					play(GodFather);
					condition = false;
					break;
				case 2:
					dump();
					condition = false;
					break;
				case 3:
					return;
				default:
					break;
			}
		}
	}
}
//=====================================================================
int Akinator :: IsLeft(Node* CurNode) {
	if(!(CurNode->father))
		return -1;
	if(CurNode->father->left == CurNode)
		return 1;
	else
		return 0;
}




#undef CHECK_SYMBOL
#undef SCANF_VALUE

