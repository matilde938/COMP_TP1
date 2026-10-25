#include <stdio.h>
#include "ast.h"
#include "parser.h"

Program* root = NULL;

// uso: ./pascal [ficheiro.pas]; sem argumento lê do stdin
int main(int argc, char** argv) {
  --argc; ++argv;
  if (argc != 0) {
    yyin = fopen(*argv, "r");
    if (!yyin) {
      printf("'%s': could not open file\n", *argv);
      return 1;
    }
  } //  yyin = stdin
  if (yyparse() == 0) {
    printProgram(root);
    return 0;
  }
  return 1; // erro de sintaxe (já indicado pelo yyerror)
}
