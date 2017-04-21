enum Types {
	TYPE_CONST,
	TYPE_VAR,
	TYPE_ACTION
};
enum FUNCTIONS {
	FUNC_ADD,
	FUNC_SUB,
	FUNC_MUL,
	FUNC_DIV,
	FUNC_DEG,
	FUNC_COS,
	FUNC_SIN,
	FUNC_LN
};
enum Tree_Status {
	STATUS_OK,
	STATUS_NOT_OK
};
enum {MAX_STR_LEN = 11};

#define FOPEN(file, name, type) { file = fopen(name, type); \
	if(!file) {												\
		perror(name);										\
		exit(1);											\
	}														\
}
#define FCLOSE(file)	\
{						\
	fclose(file);		\
	file = NULL;		\
}
enum Passes {
	PASS_POSTFIX,
	PASS_PREFIX,
	PASS_INFIX
};
enum {CHILDREN_QT = 10};
