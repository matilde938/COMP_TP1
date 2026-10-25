// AST constructor functions

#include <stdlib.h> // for malloc
#include "ast.h" // AST header

// ---------- expressions ----------

Expr* ast_integer(int v) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_INTEGER;
  node->attr.value = v;
  return node;
}

Expr* ast_boolean(int v) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_BOOLEAN;
  node->attr.value = v;
  return node;
}

Expr* ast_variable(char* name) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_VARIABLE;
  node->attr.name = name;
  return node;
}

Expr* ast_string(char* text) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_STRING;
  node->attr.text = text;
  return node;
}

Expr* ast_operation
(int operator, Expr* left, Expr* right) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_OPERATION;
  node->attr.op.operator = operator;
  node->attr.op.left = left;
  node->attr.op.right = right;
  return node;
}

Expr* ast_unary(int operator, Expr* child) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_UNARY;
  node->attr.unary.operator = operator;
  node->attr.unary.child = child;
  return node;
}

// ---------- commands ----------

Cmd* ast_assign(char* var, Expr* expr) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_ASSIGN;
  node->attr.assign.var = var;
  node->attr.assign.expr = expr;
  return node;
}

Cmd* ast_if(Expr* cond, Cmd* then_cmd, Cmd* else_cmd) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_IF;
  node->attr.ifthen.cond = cond;
  node->attr.ifthen.then_cmd = then_cmd;
  node->attr.ifthen.else_cmd = else_cmd;
  return node;
}

Cmd* ast_while(Expr* cond, Cmd* body) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_WHILE;
  node->attr.loop.cond = cond;
  node->attr.loop.body = body;
  return node;
}

Cmd* ast_seq(Cmd* first, Cmd* second) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_SEQ;
  node->attr.seq.first = first;
  node->attr.seq.second = second;
  return node;
}

Cmd* ast_write(ExprList* args, int newline) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_WRITE;
  node->attr.write.args = args;
  node->attr.write.newline = newline;
  return node;
}

Cmd* ast_read(IdList* vars, int newline) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_READ;
  node->attr.read.vars = vars;
  node->attr.read.newline = newline;
  return node;
}

// ---------- lists, declarations and program ----------

ExprList* ast_exprlist(Expr* head, ExprList* tail) {
  ExprList* node = (ExprList*) malloc(sizeof(ExprList));
  node->head = head;
  node->tail = tail;
  return node;
}

IdList* ast_idlist(char* head, IdList* tail) {
  IdList* node = (IdList*) malloc(sizeof(IdList));
  node->head = head;
  node->tail = tail;
  return node;
}

Decl* ast_decl(IdList* vars, int type, Decl* next) {
  Decl* node = (Decl*) malloc(sizeof(Decl));
  node->vars = vars;
  node->type = type;
  node->next = next;
  return node;
}

Program* ast_program(char* name, Decl* decls, Cmd* body) {
  Program* node = (Program*) malloc(sizeof(Program));
  node->name = name;
  node->decls = decls;
  node->body = body;
  return node;
}
