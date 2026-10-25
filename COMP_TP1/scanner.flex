%{
// HEADERS
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// erro lexical: indica a linha e termina
static void lexError(const char* msg) {
  fprintf(stderr, "Linha %d: %s - '%s'\n", yylineno, msg, yytext);
  exit(1);
}
%}

/* yylineno: o flex conta as linhas; caseless: Pascal não distingue maiúsculas (BEGIN = begin) */
%option noyywrap yylineno caseless nounput noinput

%%
[ \t\r\n]+ {  }                         /* ignora espaços e mudanças de linha */
"{"[^}]*"}" {  }                        /* ignora comentários { ... } */
"(*"([^*]|"*"+[^*)])*"*"+")" {  }       /* ignora comentários (* ... *) */
"//".* {  }                             /* ignora comentários de linha */
"{"[^}]* {                              /* '{' sem '}' até ao fim do ficheiro */
  char* p;                              /* volta à linha onde o comentário abriu */
  for (p = yytext; *p; p++) if (*p == '\n') yylineno--;
  yytext[1] = '\0';
  lexError("comentario nao fechado");
}
"(*" { lexError("comentario nao fechado"); }

 /* palavras reservadas: antes do ID para ganharem em caso de empate (first match) */
"program" { return PROGRAM; }
"var" { return VAR; }
"integer" { return INTEGER; }
"boolean" { return BOOLEAN; }
"begin" { return BEGIN_T; }
"end" { return END; }
"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"while" { return WHILE; }
"do" { return DO; }
"write" { return WRITE; }
"writeln" { return WRITELN; }
"read" { return READ; }
"readln" { return READLN; }
"true" { return TRUE; }
"false" { return FALSE; }
"div" { return DIV; }
"mod" { return MOD; }
"and" { return AND; }
"or" { return OR; }
"not" { return NOT; }

 /* operadores de 2 caracteres: ganham aos de 1 (longest match) */
":=" { return ASSIGN; }
"<>" { return NEQ; }
"<=" { return LE; }
">=" { return GE; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MULT; }
"/" { return DIVIDE; }
"=" { return EQ; }
"<" { return LT; }
">" { return GT; }
"(" { return LPAREN; }
")" { return RPAREN; }
";" { return SEMICOLON; }
":" { return COLON; }
"," { return COMMA; }
"." { return DOT; }

 /* tokens com valor: o texto é copiado porque o yytext é reescrito a cada token */
[0-9]+ {
  yylval.intValue = atoi(yytext);
  return INT;
}
'([^'\n]|'')*' {                        /* '' é uma plica dentro da string */
  yylval.strValue = strdup(yytext);
  return STR;
}
[a-zA-Z_][a-zA-Z0-9_]* {
  yylval.strValue = strdup(yytext);
  return ID;
}
. { lexError("caracter invalido"); }
%%
