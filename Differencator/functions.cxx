Node* Differentiator :: c(Node* CurNode) {
	Node* CopyNode = new Node(" ", NULL);
	CopyNode->type = CurNode->type;
	CopyNode->data = CurNode->data;
	CopyNode->ChildQt = CurNode->ChildQt;
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i]) {
			CopyNode->children[i] = c(CurNode->children[i]);
			CopyNode->children[i]->parent = CopyNode;
		}
	}
	return CopyNode;
}
//======================================================================
Node* Differentiator :: d_const(Node* CurNode) {
	Node* DifNode = new Node("0", NULL);
	return DifNode;
}
Node* Differentiator :: d_var(Node* CurNode) {
	Node* DifNode = new Node("1", NULL);
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_add(Node* CurNode) {
	Node* DifNode = new Node("+", NULL);
	DifNode->left = d(CurNode->left);
	DifNode->right = d(CurNode->right);
	DifNode->left->parent = DifNode->right->parent = DifNode;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_sub(Node* CurNode) {
	Node* DifNode = new Node("-", NULL);
	DifNode->left = d(CurNode->left);
	DifNode->right = d(CurNode->right);
	DifNode->left->parent = DifNode->right->parent = DifNode;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_mul(Node* CurNode) {
	Node* DifNode = new Node("+", NULL);
	Node* Left = new Node("*", DifNode);
	Node* Right = new Node("*", DifNode);
	DifNode->left = Left;
	DifNode->right = Right;
	Left->left = d(CurNode->left);
	Left->right  = c(CurNode->right);
	Left->left->parent = Left->right->parent = Left;
	Right->left = c(CurNode->left);
	Right->right = d(CurNode->right);
	Right->left->parent = Right->right->parent = Right;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_div(Node* CurNode) {
	Node* DifNode = new Node("/", NULL);
	Node* Left = new Node("-", DifNode);
	Node* Right = new Node("^", DifNode);
	DifNode->left = Left;
	DifNode->right = Right;
	Node* LeftLeft = new Node("*", Left);
	Node* LeftRight = new Node("*", Left);
	Left->left = LeftLeft;
	Left->right = LeftRight;
	Node* RightRight = new Node("2", NULL);
	Right->left = c(CurNode->right);
	Right->right = RightRight;
	Right->left->parent = Right->right->parent = Right;
	LeftLeft->left = d(CurNode->left);
	LeftLeft->right = c(CurNode->right);
	LeftLeft->left->parent = LeftLeft->right->parent = LeftLeft;
	LeftRight->left = c(CurNode->left);
	LeftRight->right = d(CurNode->right);
	LeftRight->left->parent = LeftRight->right->parent = LeftRight;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_deg(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	Node* Left = new Node("*", NULL);
	Node* Right = d(CurNode->left);
	DifNode->left = Left;
	DifNode->right = Right;
	Left->parent = Right->parent = DifNode;
	Node* LeftLeft = new Node("^", NULL);
	Node* LeftRight = c(CurNode->right);
	Left->left = LeftLeft;
	Left->right = LeftRight;
	LeftLeft->parent = LeftRight->parent = Left;
	Node* LeftLeftRight = c(CurNode->right);
	(LeftLeftRight->data)--;
	LeftLeft->left = c(CurNode->left);
	LeftLeft->right = LeftLeftRight;
	LeftLeft->left->parent = LeftLeft->right->parent = LeftLeft;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_ln(Node* CurNode) {
	Node* DifNode = new Node("/", NULL);
	DifNode->left = d(CurNode->left);
	DifNode->right = c(CurNode->left);
	DifNode->left->parent = DifNode->right->parent = DifNode;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_cos(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	DifNode->left = new Node("*", DifNode);
	DifNode->right = new Node("sin", DifNode);
	DifNode->left->left = new Node("-1", DifNode->left);
	DifNode->left->right = d(CurNode->left);
	DifNode->left->right->parent = DifNode->left;
	DifNode->right->left = c(CurNode->left);
	DifNode->right->left->parent = DifNode->right;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_sin(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	DifNode->left = new Node("cos", DifNode);
	DifNode->right = d(CurNode->left);
	DifNode->right->parent = DifNode;
	DifNode->left->left = c(CurNode->left);
	DifNode->left->left->parent = DifNode->left;
	return DifNode;	
}
//======================================================================
#define LINK_PARENTS {						\
	if(cur->parent) {						\
		CurNode->parent = cur->parent;		\
		if(IsLeft(cur))						\
			cur->parent->left = CurNode;	\
		else								\
			cur->parent->right = CurNode;	\
	}										\
}
//======================================================================
Node* Differentiator :: simple_add_zero(Node* CurNode) {
	printf("\nxui\n");
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 0)
		CurNode = c(CurNode->right);
	else if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0)
		CurNode = c(CurNode->left);
	LINK_PARENTS
	delete cur;
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_sub_zero(Node* CurNode) {
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0) {
		Node* cur = CurNode;
		CurNode = c(CurNode->left);
		LINK_PARENTS
		delete cur;
	}
	else {
		CurNode->data = FUNC_MUL;
		CurNode->left->type = TYPE_CONST;
		CurNode->left->data = -1;
		CurNode->left->parent = CurNode->right->parent = CurNode;
	}
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_mul_zero(Node* CurNode) {
	Node* cur = CurNode;
	CurNode = new Node("0", NULL);
	LINK_PARENTS
	delete cur;
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_mul_one(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 1)
		CurNode = c(CurNode->right);
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 1)
		CurNode = c(CurNode->left);
	LINK_PARENTS
	delete cur;
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_deg_zero(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0)
		CurNode = new Node("1", NULL);
	else if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 0)
		CurNode = new Node("0", NULL);
	LINK_PARENTS
	delete cur;
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_deg_one(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 1)
		CurNode = new Node("1", NULL);
	else if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 1)
		CurNode = c(CurNode->left);
	LINK_PARENTS
	delete cur;
	return CurNode;
}
