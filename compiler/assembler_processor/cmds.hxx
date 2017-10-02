#define EBP stack_.data_ + registers_[REG_EBP]

#define MAKE_ARG(arg_name, i)                                   \
    int* arg_name;                                              \
    switch(arg_types[i]) {                                      \
        case ARG_NUMBER:                                        \
            arg_name = arguments + i;                           \
            break;                                              \
        case ARG_REG:                                           \
            if(arguments[i] == REG_ESP)                         \
                arg_name = &(stack_.size_);                     \
            else                                                \
                arg_name = registers_ + arguments[i];           \
            break;                                              \
        case ARG_STACK:                                         \
            arg_name = EBP + arguments[i];                      \
            break;                                              \
        default:                                                \
            printf("\nInvalid arg_type in command № %d\n", ip_);\
            exit(1);                                            \
    }

/*0*/DEF_CMD(MOV, 2,
    MAKE_ARG(arg0, 0)
    MAKE_ARG(arg1, 1)
    *arg0 = *arg1;
    move_ip();
)
/*1*/DEF_CMD(PUSH, 1,
    MAKE_ARG(arg, 0)
    stack_.push(*arg);
    move_ip();
)
/*2*/DEF_CMD(POP, 1,
    MAKE_ARG(arg, 0)
    *arg = stack_.pop();
    move_ip();
)
#define AREFMETIC(Name, action)	                \
DEF_CMD(Name, 2, 				                \
    MAKE_ARG(arg0, 0)                           \
    MAKE_ARG(arg1, 1)                           \
    *arg0 = *arg0 action *arg1;	                \
    move_ip();					                \
)
/*3*/AREFMETIC(ADD, +)
/*4*/AREFMETIC(SUB, -)
/*5*/AREFMETIC(MUL, *)
/*6*/AREFMETIC(DIV, /)
#undef AREFMETIC
/*7*/DEF_CMD(INC, 1,
    MAKE_ARG(arg, 0)
    (*arg)++;
    move_ip();
)
/*8*/DEF_CMD(DEC, 1, 
    MAKE_ARG(arg, 0)
    (*arg)--;
    move_ip();
)
/*9*/DEF_CMD(JMP, 1, move_ip(arguments[0]);)
#define JMP(Name, compare)								\
DEF_CMD(Name, 3, 										\
	MAKE_ARG(arg0, 0)                                   \
    MAKE_ARG(arg1, 1)                                   \
    if(*arg0 compare *arg1)	                            \
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
/*16*/DEF_CMD(IN, 1,   
    MAKE_ARG(arg, 0);
    scanf("%d", arg);
    move_ip();

)
/*17*/DEF_CMD(OUT, 1,
    MAKE_ARG(arg, 0);
    printf("\n%d ", *arg);
    move_ip();
)
/*18*/DEF_CMD(CALL, 1, 
    stack_.push(ip_ + 1);
    move_ip(arguments[0]);
)
/*19*/DEF_CMD(RET, 0, move_ip(stack_.pop());)
/*20*/DEF_CMD(HALT, 0, fuckTheProcessor = true;)



#undef MAKE_ARG
#undef EBP
