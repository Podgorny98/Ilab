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
//======================================================================
#define CHECK_CHAR(a) {						\
	fscanf(f, "%c", &CurChar);				\
	if(CurChar != a) {						\
		printf("Wrong input construction");	\
		exit(1);							\
	}										\
}

enum {MAX_STR_LEN = 256};
enum TREE_ERRS {
	ERR_NO,
	ERR_NULL_PTR,
	ERR_LEFT_PTR,
	ERR_RIGHT_PTR,
	ERR_FATHER_PTR,
	ERR_WRONG_DATA
};
