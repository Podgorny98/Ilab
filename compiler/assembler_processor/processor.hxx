enum {
	CMD_LEN = 4,	        //команда c пробелом занимает в prod_mem_ 4 позиции
    ARG_TYPE_LEN = 2,       //тип аргумента с пробелом занимает 2 позиции
	ARG_LEN = 11	        //аргумент с пробелом занимает в prog_mem_ 11 позиций
};
enum {REGISTERS_QT = 16};

enum {
    REG_ESP = 15,
    REG_EBP = 14
};

int get_file_size(FILE* f);
//=============================================================
class Processor {
private:
	Stack stack_;
	int registers_[REGISTERS_QT];
	char* prog_mem_;
	int ip_;
	int sp_;
	int cmd_;		// Команда, обрабатываемая в данный момент
	int file_size_; 
	int get_arg_qt(int num);
	void read_asm_file(FILE* asm_file);
	void move_ip();
	void move_ip(int new_ip);
	int get_file_size(FILE* f);
public:
	Processor(FILE* asm_file);
	~Processor();
	void execute();
	void dump();
	void reg_dump();
	void stack_dump();
};

