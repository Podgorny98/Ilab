DEF_DATA("+", TYPE_ACTION, FUNC_ADD, 2, d_add, 0)
DEF_DATA("-", TYPE_ACTION, FUNC_SUB, 2, d_sub, 0)
DEF_DATA("*", TYPE_ACTION, FUNC_MUL, 2, d_mul, 1)
DEF_DATA("/", TYPE_ACTION, FUNC_DIV, 2, d_div, 1)
DEF_DATA("^", TYPE_ACTION, FUNC_DEG, 2, d_deg, 2)
DEF_DATA("cos", TYPE_ACTION, FUNC_COS, 1, d_cos, 3)
DEF_DATA("sin", TYPE_ACTION, FUNC_SIN, 1, d_sin, 3)
DEF_DATA("ln", TYPE_ACTION, FUNC_LN, 1, d_ln, 3)
DEF_DATA("x", TYPE_VAR, FUNC_VAR, 0, d_var, 4)
DEF_DATA(" ", TYPE_CONST, FUNC_CONST, 0, d_const, 4)
