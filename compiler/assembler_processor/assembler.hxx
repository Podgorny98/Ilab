enum {MAX_OUT_STR_LEN = 100000};
enum {MAX_LABELS_QT = 50};
enum {MAX_CMD_LEN = 100};


class Assembler {
private:
	FILE* fin;
	FILE* fout;
    char* labels[MAX_LABELS_QT];
	int labels_nums[MAX_LABELS_QT];
    int labels_qt;
	ASM_CMDS get_num(const char* cmdstr);
	int get_arg_qt(ASM_CMDS code);
	void first_passing();
	void second_passing();
    void get_args(ASM_CMDS cmd_num, int com_num, int* offset);
public:
	Assembler(FILE* in, FILE* out);
	~Assembler();
	void printf_out();
	void file_analisis();
	char outstr[MAX_OUT_STR_LEN];
};

