CC=g++
SOURCES=main.c lexical_analyzer/lexical_analyzer.c lexical_analyzer/token_list.c syntactical_analyzer/syntactical_analyzer.c syntactical_analyzer/syntactical_tree.c semantic_analyzer/semantic_analyzer.c assembler_processor/assembler.cxx assembler_processor/processor.cxx assembler_processor/stack.cxx
HEADERS=
SUPER_OPTIONS=-lasan -O2 -g -Wall -Wextra  -Waggressive-loop-optimizations  -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion  -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wno-missing-declarations -Wopenmp-simd -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wstack-usage=8192 -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -fcheck-new -fstack-check -fstack-protector -fstrict-overflow -fchkp-first-field-has-own-bounds -fchkp-narrow-to-innermost-array -flto-odr-type-merging -fno-omit-frame-pointer -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -fPIE -pie
USUAL_OPTIONS=-Wall -ansi -Wfatal-errors

all: COMPILER SUPERCOMPILER

COMPILER: $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(USUAL_OPTIONS) -o $@

SUPERCOMPILER: $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(SUPER_OPTIONS) -o $@
