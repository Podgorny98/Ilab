DEF_DATA("+", TYPE_ACTION, FUNC_ADD, d_add, 0)
DEF_DATA("-", TYPE_ACTION, FUNC_SUB, d_sub, 0)
DEF_DATA("*", TYPE_ACTION, FUNC_MUL, d_mul, 1)
DEF_DATA("/", TYPE_ACTION, FUNC_DIV, d_div, 1)
DEF_DATA("^", TYPE_ACTION, FUNC_DEG, d_deg, 2)
DEF_DATA("cos", TYPE_ACTION, FUNC_COS, d_cos, 3)
DEF_DATA("sin", TYPE_ACTION, FUNC_SIN, d_sin, 3)
DEF_DATA("ln", TYPE_ACTION, FUNC_LN, d_ln, 3)
DEF_DATA("x", TYPE_VAR, 1, d_var, 4)
DEF_DATA(" ", TYPE_CONST,1 , d_const, 4)
