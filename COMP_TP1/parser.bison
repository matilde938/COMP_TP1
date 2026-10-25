// Parser (Bison) para um subconjunto de Pascal: as ações constroem a AST

// Tokens (o texto entre aspas é o nome usado nas mensagens de erro)
%token
  PROGRAM "program" VAR "var" INTEGER "integer" BOOLEAN "boolean"
  BEGIN_T "begin" END "end" IF "if" THEN "then" ELSE "else"
  WHILE "while" DO "do" WRITE "write" WRITELN "writeln"
  READ "read" READLN "readln" TRUE "true" FALSE "false"
  ASSIGN ":=" PLUS "+" MINUS "-" MULT "*" DIVIDE "/"
  DIV "div" MOD "mod" AND "and" OR "or" NOT "not"
  EQ "=" NEQ "<>" LT "<" LE "<=" GT ">" GE ">="
  LPAREN "(" RPAREN ")" SEMICOLON ";" COLON ":" COMMA "," DOT "."
  INT ID STR

// Operator associativity & precedence (cada linha tem mais prioridade do que a anterior)
%precedence THEN                // THEN < ELSE: o else liga-se ao if mais próximo
%precedence ELSE
%nonassoc EQ NEQ LT LE GT GE    // a < b < c é erro
%left PLUS MINUS OR
%left MULT DIVIDE DIV MOD AND

// Root-level grammar symbol
%start program;

// Mensagens de erro com o token encontrado e os esperados
%define parse.error verbose

// Types/values in association to grammar symbols.
%union {
  int intValue;
  char* strValue;
  Expr* exprValue;
  ExprList* exprListValue;
  IdList* idListValue;
  Cmd* cmdValue;
  Decl* declValue;
}

%type <intValue> INT type
%type <strValue> ID STR
%type <exprValue> expr factor arg
%type <exprListValue> args
%type <idListValue> ids
%type <cmdValue> cmds cmd
%type <declValue> decls decl

// Use "%code requires" to make declarations go
// into both parser.c and parser.h
%code requires {
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern void yyerror(const char* msg);
extern Program* root;
}

%%
program: PROGRAM ID SEMICOLON decls BEGIN_T cmds END DOT { root = ast_program($2, $4, $6); }
  ;

// declarações de variáveis
decls:
  %empty { $$ = NULL; }         // programa sem 'var'
  | VAR decl { $$ = $2; }
  ;
decl:
  ids COLON type SEMICOLON { $$ = ast_decl($1, $3, NULL); }
  | ids COLON type SEMICOLON decl { $$ = ast_decl($1, $3, $5); }
  ;
type:
  INTEGER { $$ = INTEGER; }
  | BOOLEAN { $$ = BOOLEAN; }
  ;
ids:
  ID { $$ = ast_idlist($1, NULL); }
  | ID COMMA ids { $$ = ast_idlist($1, $3); }
  ;

// comandos separados por ';'; os comandos vazios (NULL) não entram na sequência
cmds:
  cmd { $$ = $1; }
  | cmd SEMICOLON cmds { $$ = !$1 ? $3 : !$3 ? $1 : ast_seq($1, $3); }
  ;
cmd:
  %empty { $$ = NULL; }         // aceita ';' antes de 'end'
  | ID ASSIGN expr { $$ = ast_assign($1, $3); }
  | IF expr THEN cmd { $$ = ast_if($2, $4, NULL); }
  | IF expr THEN cmd ELSE cmd { $$ = ast_if($2, $4, $6); }
  | WHILE expr DO cmd { $$ = ast_while($2, $4); }
  | BEGIN_T cmds END { $$ = $2; }
  | WRITE LPAREN args RPAREN { $$ = ast_write($3, 0); }
  | WRITELN LPAREN args RPAREN { $$ = ast_write($3, 1); }
  | WRITELN LPAREN RPAREN { $$ = ast_write(NULL, 1); }
  | WRITELN { $$ = ast_write(NULL, 1); }
  | READ LPAREN ids RPAREN { $$ = ast_read($3, 0); }
  | READLN LPAREN ids RPAREN { $$ = ast_read($3, 1); }
  ;
args:
  arg { $$ = ast_exprlist($1, NULL); }
  | arg COMMA args { $$ = ast_exprlist($1, $3); }
  ;
arg:
  expr { $$ = $1; }
  | STR { $$ = ast_string($1); }
  ;

// expressões com operadores binários: a ambiguidade é resolvida pelas precedências
expr:
  factor { $$ = $1; }
  | expr PLUS expr { $$ = ast_operation(PLUS, $1, $3); }
  | expr MINUS expr { $$ = ast_operation(MINUS, $1, $3); }
  | expr MULT expr { $$ = ast_operation(MULT, $1, $3); }
  | expr DIVIDE expr { $$ = ast_operation(DIVIDE, $1, $3); }
  | expr DIV expr { $$ = ast_operation(DIV, $1, $3); }
  | expr MOD expr { $$ = ast_operation(MOD, $1, $3); }
  | expr AND expr { $$ = ast_operation(AND, $1, $3); }
  | expr OR expr { $$ = ast_operation(OR, $1, $3); }
  | expr EQ expr { $$ = ast_operation(EQ, $1, $3); }
  | expr NEQ expr { $$ = ast_operation(NEQ, $1, $3); }
  | expr LT expr { $$ = ast_operation(LT, $1, $3); }
  | expr LE expr { $$ = ast_operation(LE, $1, $3); }
  | expr GT expr { $$ = ast_operation(GT, $1, $3); }
  | expr GE expr { $$ = ast_operation(GE, $1, $3); }
  ;
// factor: operandos e operadores unários (aplicam-se só a um factor)
factor:
  INT { $$ = ast_integer($1); }
  | ID { $$ = ast_variable($1); }
  | TRUE { $$ = ast_boolean(1); }
  | FALSE { $$ = ast_boolean(0); }
  | LPAREN expr RPAREN { $$ = $2; }
  | MINUS factor { $$ = ast_unary(MINUS, $2); }
  | NOT factor { $$ = ast_unary(NOT, $2); }
  ;
%%

void yyerror(const char* err) {
  fprintf(stderr, "Linha %d: %s - '%s'\n", yylineno, err, yytext);
}
