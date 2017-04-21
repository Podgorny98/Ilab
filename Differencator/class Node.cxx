class Node {
public:
	Types type;
	TreeNodeType data;
	Node* parent;
	Node* children[CHILDREN_QT];
	int ChildQt;
	Node(const char* str, Node* par);
	~Node();
	int GetPriority();
};
//======================================================================
Node :: Node(const char* str, Node* par) {
#define DEF_DATA(Str, Type, Data, function, priority) {	\
	if(!strcmp(str, Str)) {						\
		type = Type;							\
		data = Data;							\
		for(int i = 0; i < CHILDREN_QT; i++)	\
			children[i] = NULL;					\
		parent = par;							\
		ChildQt = 0;							\
		return;									\
	}											\
}
#include "def_data.cxx"
#undef DEF_DATA
	type = TYPE_CONST;
	data = atof(str);
	ChildQt = 0;
	for(int i = 0; i < CHILDREN_QT; i++)
		children[i] = NULL;
	parent = par;
}
//======================================================================
Node :: ~Node() {
	for(int i = 0; i < ChildQt; i++) {
		if(children[i]) {
			delete children[i];
			children[i] = NULL;
		}
	}
	parent = NULL;
	ChildQt = 0;
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
