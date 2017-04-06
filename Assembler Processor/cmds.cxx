/*0*/DEF_CMD(MOV, 2, registers_[arguments[0]] = arguments[1]; move_ip();)
/*1*/DEF_CMD(PUSH, 1, stack_.push(arguments[0]); move_ip();)
/*2*/DEF_CMD(POP, 1, registers_[arguments[0]] = stack_.pop(); move_ip();)
#define AREFMETIC(Name, action)	\
DEF_CMD(Name, 0, 				\
	int x = stack_.pop();		\
	int y = stack_.pop();		\
	stack_.push(x action y);	\
	move_ip();					\
)
/*3*/AREFMETIC(ADD, +)
/*4*/AREFMETIC(SUB, -)
/*5*/AREFMETIC(MUL, *)
/*6*/AREFMETIC(DIV, /)
#undef AREFMETIC
/*7*/DEF_CMD(INC, 1, registers_[arguments[0]]++; move_ip();)
/*8*/DEF_CMD(DEC, 1, registers_[arguments[0]]--; move_ip();)
/*9*/DEF_CMD(JMP, 1, move_ip(arguments[0]);)
#define JMP(Name, compare)								\
DEF_CMD(Name, 3, 										\
	if(registers_[arguments[0]] compare arguments[1])	\
		move_ip(arguments[2]);							\
	else 												\
		move_ip();										\
)
/*10*/JMP(JG, >)
/*11*/JMP(JGE, >=)
/*12*/JMP(JL, <)
/*13*/JMP(JLE, <=)
/*14*/JMP(JE, ==)
/*15*/JMP(JNE, !=)
#undef JMP
#define REG_AREFMETIC(Name, action)																\
DEF_CMD(Name, 2,																				\
registers_[arguments[0]] = registers_[arguments[0]] action registers_[arguments[1]]; move_ip();)
/*16*/REG_AREFMETIC(RADD, +)
/*17*/REG_AREFMETIC(RSUB, -)
/*18*/REG_AREFMETIC(RMUL, *)
/*19*/REG_AREFMETIC(RDIV, /)
#undef REG_AREFMETIC
/*20*/DEF_CMD(MOVR, 2, registers_[arguments[0]] = registers_[arguments[1]]; move_ip();)
/*21*/DEF_CMD(PUSHR, 1, stack_.push(registers_[arguments[0]]); move_ip();)
/*22*/DEF_CMD(IN, 1, scanf("%d", &registers_[arguments[0]]); move_ip();)
/*23*/DEF_CMD(OUT, 1, printf("\n%d ", registers_[arguments[0]]); move_ip();)
/*24*/DEF_CMD(CALL, 1, call_stack.push(ip_ + 1); move_ip(arguments[0]);)
/*25*/DEF_CMD(RET, 0, move_ip(call_stack.pop());)
/*26*/DEF_CMD(HALT, 0, fuckTheProcessor = true;)






