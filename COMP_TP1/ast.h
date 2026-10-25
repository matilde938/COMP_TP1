
// AST definitions
#ifndef __ast_h__
#define __ast_h__

// AST for expressions (aritméticas, relacionais e booleanas)
struct _Expr {
  enum {
    E_INTEGER,      // 123
    E_BOOLEAN,      // true, false
    E_VARIABLE,     // x
    E_STRING,       // 'texto' (só nos argumentos do write)
    E_OPERATION,    // e1 op e2
    E_UNARY         // -e, not e
  } kind;
  union {
    int value;      // for integer and boolean values
    char* name;     // for variables
    char* text;     // for strings (com as plicas)
    struct {
      int operator; // PLUS, MINUS, etc (código do token)
      struct _Expr* left;
      struct _Expr* right;
    } op;           // for binary expressions
    struct {
      int operator; // MINUS ou NOT
      struct _Expr* child;
    } unary;        // for unary expressions
  } attr;
};

typedef struct _Expr Expr; // Convenience typedef

// lista de expressões (argumentos do write)
struct _ExprList {
  Expr* head;
  struct _ExprList* tail;
};

typedef struct _ExprList ExprList;

// lista de identificadores (read e declarações)
struct _IdList {
  char* head;
  struct _IdList* tail;
};

typedef struct _IdList IdList;

// AST for commands
struct _Cmd {
  enum {
    C_ASSIGN,       // x := e
    C_IF,           // if e then c1 [else c2]
    C_WHILE,        // while e do c
    C_SEQ,          // c1 ; c2
    C_WRITE,        // write(...) / writeln(...)
    C_READ          // read(...) / readln(...)
  } kind;
  union {
    struct {
      char* var;
      Expr* expr;
    } assign;
    struct {
      Expr* cond;
      struct _Cmd* then_cmd;
      struct _Cmd* else_cmd;   // NULL se não houver else
    } ifthen;
    struct {
      Expr* cond;
      struct _Cmd* body;
    } loop;
    struct {
      struct _Cmd* first;
      struct _Cmd* second;
    } seq;
    struct {
      ExprList* args;
      int newline;             // 1 para writeln
    } write;
    struct {
      IdList* vars;
      int newline;             // 1 para readln
    } read;
  } attr;
};

typedef struct _Cmd Cmd;

// declarações: uma linha 'ids : tipo;' por nó
struct _Decl {
  IdList* vars;
  int type;         // INTEGER ou BOOLEAN (código do token)
  struct _Decl* next;
};

typedef struct _Decl Decl;

// programa: nome, declarações e corpo
struct _Program {
  char* name;
  Decl* decls;
  Cmd* body;
};

typedef struct _Program Program;

// Constructor functions (see implementation in ast.c)
Expr* ast_integer(int v);
Expr* ast_boolean(int v);
Expr* ast_variable(char* name);
Expr* ast_string(char* text);
Expr* ast_operation(int operator, Expr* left, Expr* right);
Expr* ast_unary(int operator, Expr* child);

Cmd* ast_assign(char* var, Expr* expr);
Cmd* ast_if(Expr* cond, Cmd* then_cmd, Cmd* else_cmd);
Cmd* ast_while(Expr* cond, Cmd* body);
Cmd* ast_seq(Cmd* first, Cmd* second);
Cmd* ast_write(ExprList* args, int newline);
Cmd* ast_read(IdList* vars, int newline);

ExprList* ast_exprlist(Expr* head, ExprList* tail);
IdList* ast_idlist(char* head, IdList* tail);
Decl* ast_decl(IdList* vars, int type, Decl* next);
Program* ast_program(char* name, Decl* decls, Cmd* body);

// Print functions (see implementation in printAbsTree.c)
void printExpr(Expr* exp, int level);
void printCmd(Cmd* command, int level);
void printProgram(Program* prog);

#endif
