void Differentiator :: dump() {
	FILE* DumpFile;
	FOPEN(DumpFile, "dumpfile.gv", "w")
	fprintf(DumpFile,"digraph graf{\n"); 
	NodeDump(tree,DumpFile); 
	fprintf(DumpFile,"}"); 
	FCLOSE(DumpFile); 
	system ("dot dumpfile.gv -Tpng -o dumpfile.png");
	system ("xdot dumpfile.gv");
}
//======================================================================
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
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i]) {
			fprintf (DumpFile, "treeNode_%p->treeNode_%p\n", CurNode, CurNode->children[i]);
			NodeDump (CurNode->children[i], DumpFile); 
		}
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

