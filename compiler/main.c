#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "enums"
#include "lexical_analyzer/token_list.h"
#include "lexical_analyzer/lexical_analyzer.h"
#include "syntactical_analyzer/syntactical_tree.h"
#include "syntactical_analyzer/syntactical_analyzer.h"
#include "semantic_analyzer/semantic_analyzer.h"
#include "assembler_processor/assembler_processor.hxx"
#include "assembler_processor/assembler.hxx"
#include "assembler_processor/stack.hxx"
#include "assembler_processor/processor.hxx"

void compile(FILE* programm);

int main(int argc, char **argv) {
    FILE* programm;
    if(argc != 2) {
        printf("\nMust be 1 argument - name of compiling file\n");
        exit(1);
    }
    programm = fopen(argv[1], "r");
    if(programm == NULL) {
        perror(argv[1]);
        exit(2);
    }
    compile(programm);
    fclose(programm);
    return 0;
}

void compile(FILE* programm) {
    struct token_list * list = token_ctor();
    lexical_analyzer(programm, list);
  //  token_list_dump(list);
    tree_node* syntactical_tree = syntactical_analyzer(list);
  //  tree_dump(syntactical_tree);
    semantic_analyzer(syntactical_tree);
    tree_dtor(syntactical_tree);    
    token_detor(list);

    FILE* asm_in;
    FILE* asm_out;
    asm_in = fopen("asm_file", "r");
    if(asm_in == NULL) {
        perror("asm_file");
        exit(2);
    }
    asm_out = fopen("machine_code", "w");
    if(asm_out == NULL) {
        perror("machine_code");
        exit(2);
    }
    Assembler a(asm_in, asm_out);
    a.file_analisis();
    a.printf_out();
    asm_out = fopen("machine_code", "r");
    if(asm_out == NULL) {
        perror("machine_code");
        exit(2);
    }
    Processor proc(asm_out);
    proc.execute();
}
