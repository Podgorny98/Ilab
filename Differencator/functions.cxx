Node* Differentiator :: copy(Node* CurNode) {
	Node* CopyNode = new Node(" ", NULL);
	CopyNode->type = CurNode->type;
	CopyNode->data = CurNode->data;
	CopyNode->ChildQt = CurNode->ChildQt;
	CopyNode->parent = NULL;
	for(int i = 0; i < CurNode->ChildQt; i++) {
		if(CurNode->children[i]) {
			CopyNode->children[i] = copy(CurNode->children[i]);
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
	DifNode->left = dif(CurNode->left);
	DifNode->right = dif(CurNode->right);
	DifNode->left->parent = DifNode->right->parent = DifNode;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_sub(Node* CurNode) {
	Node* DifNode = new Node("-", NULL);
	DifNode->left = dif(CurNode->left);
	DifNode->right = dif(CurNode->right);
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
	Left->left = dif(CurNode->left);
	Left->right  = copy(CurNode->right);
	Left->left->parent = Left->right->parent = Left;
	Right->left = copy(CurNode->left);
	Right->right = dif(CurNode->right);
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
	Right->left = copy(CurNode->right);
	Right->right = RightRight;
	Right->left->parent = Right->right->parent = Right;
	LeftLeft->left = dif(CurNode->left);
	LeftLeft->right = copy(CurNode->right);
	LeftLeft->left->parent = LeftLeft->right->parent = LeftLeft;
	LeftRight->left = copy(CurNode->left);
	LeftRight->right = dif(CurNode->right);
	LeftRight->left->parent = LeftRight->right->parent = LeftRight;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_deg(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	Node* Left = new Node("*", NULL);
	Node* Right = dif(CurNode->left);
	DifNode->left = Left;
	DifNode->right = Right;
	Left->parent = Right->parent = DifNode;
	Node* LeftLeft = new Node("^", NULL);
	Node* LeftRight = copy(CurNode->right);
	Left->left = LeftLeft;
	Left->right = LeftRight;
	LeftLeft->parent = LeftRight->parent = Left;
	Node* LeftLeftRight = copy(CurNode->right);
	(LeftLeftRight->data)--;
	LeftLeft->left = copy(CurNode->left);
	LeftLeft->right = LeftLeftRight;
	LeftLeft->left->parent = LeftLeft->right->parent = LeftLeft;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_ln(Node* CurNode) {
	Node* DifNode = new Node("/", NULL);
	DifNode->left = dif(CurNode->left);
	DifNode->right = copy(CurNode->left);
	DifNode->left->parent = DifNode->right->parent = DifNode;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_cos(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	DifNode->left = new Node("*", DifNode);
	DifNode->right = new Node("sin", DifNode);
	DifNode->left->left = new Node("-1", DifNode->left);
	DifNode->left->right = dif(CurNode->left);
	DifNode->left->right->parent = DifNode->left;
	DifNode->right->left = copy(CurNode->left);
	DifNode->right->left->parent = DifNode->right;
	return DifNode;
}
//======================================================================
Node* Differentiator :: d_sin(Node* CurNode) {
	Node* DifNode = new Node("*", NULL);
	DifNode->left = new Node("cos", DifNode);
	DifNode->right = dif(CurNode->left);
	DifNode->right->parent = DifNode;
	DifNode->left->left = copy(CurNode->left);
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
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 0) {
		CurNode = copy(CurNode->right);
		LINK_PARENTS
		delete cur;
	}
	else if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0) {
		CurNode = copy(CurNode->left);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_sub_zero(Node* CurNode) {
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0) {
		Node* cur = CurNode;
		CurNode = copy(CurNode->left);
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
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 1) {
		CurNode = copy(CurNode->right);
		LINK_PARENTS
		delete cur;
	}
	else if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 1) {
		CurNode = copy(CurNode->left);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_deg_zero(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 0) {
		CurNode = new Node("1", NULL);
		LINK_PARENTS
		delete cur;
	}
	else if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 0){
		CurNode = new Node("0", NULL);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_deg_one(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 1) {
		CurNode = new Node("1", NULL);
		LINK_PARENTS
		delete cur;
	}
	else if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 1) {
		CurNode = copy(CurNode->left);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
//======================================================================
Node* Differentiator :: simple_div_zero(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->left->type == TYPE_CONST && CurNode->left->data == 0) {
		CurNode = new Node("0", NULL);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
Node* Differentiator :: simple_div_one(Node* CurNode) {
	Node* cur = CurNode;
	if(CurNode->right->type == TYPE_CONST && CurNode->right->data == 1) {
		CurNode = copy(CurNode->left);
		LINK_PARENTS
		delete cur;
	}
	return CurNode;
}
#undef LINK_PARENTS
//======================================================================
void Differentiator :: GeneralSimplification(Node* CurNode) {
	if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_DEG && CurNode->right->type == TYPE_CONST &&
	CurNode->left->type == TYPE_ACTION && CurNode->left->data == FUNC_DEG && CurNode->left->right->type == TYPE_CONST) {
		CurNode->right->data *= CurNode->left->right->data;
		Node* Left = CurNode->left;
		CurNode->left = copy(CurNode->left->left);
		CurNode->left->parent = CurNode;
		delete Left;
	}
	else if(CurNode->type == TYPE_ACTION && CurNode->data == FUNC_DIV && 
	CurNode->right->type == TYPE_ACTION && CurNode->right->data == FUNC_DIV) {
		CurNode->data = FUNC_MUL;
		Node* Left = CurNode->right->left;
		CurNode->right->left = CurNode->right->right;
		CurNode->right->right = Left;
	}
}
	
	
	
	
	
	
	
	
	
	
	
