// Print functions: imprimem a AST em pré-ordem, 2 espaços por nível

#include <stdio.h>
#include "ast.h"
#include "parser.h" // códigos dos tokens (PLUS, MINUS, ...)

static void indent(int level) {
  printf("%*s", level * 2, "");
}

// texto de um operador a partir do código do token
static const char* opName(int operator) {
  switch (operator) {
    case PLUS:   return "+";
    case MINUS:  return "-";
    case MULT:   return "*";
    case DIVIDE: return "/";
    case DIV:    return "div";
    case MOD:    return "mod";
    case AND:    return "and";
    case OR:     return "or";
    case NOT:    return "not";
    case EQ:     return "=";
    case NEQ:    return "<>";
    case LT:     return "<";
    case LE:     return "<=";
    case GT:     return ">";
    case GE:     return ">=";
    default:     return "?";
  }
}

void printExpr(Expr* exp, int level) {
  indent(level);
  switch (exp->kind) {
    case E_INTEGER:
      printf("Int %d\n", exp->attr.value);
      break;
    case E_BOOLEAN:
      printf("%s\n", exp->attr.value ? "true" : "false");
      break;
    case E_VARIABLE:
      printf("Var %s\n", exp->attr.name);
      break;
    case E_STRING:
      printf("String %s\n", exp->attr.text);
      break;
    case E_OPERATION:
      printf("%s\n", opName(exp->attr.op.operator));
      printExpr(exp->attr.op.left, level + 1);
      printExpr(exp->attr.op.right, level + 1);
      break;
    case E_UNARY:
      printf("%s\n", opName(exp->attr.unary.operator));
      printExpr(exp->attr.unary.child, level + 1);
      break;
  }
}

static void printIds(IdList* ids, int level) {
  for (; ids; ids = ids->tail) {
    indent(level);
    printf("Var %s\n", ids->head);
  }
}

// uma sequência (C_SEQ) não aparece na árvore: os seus comandos ficam ao mesmo nível
void printCmd(Cmd* command, int level) {
  ExprList* args;
  if (!command) return;  // comando vazio
  switch (command->kind) {
    case C_SEQ:
      printCmd(command->attr.seq.first, level);
      printCmd(command->attr.seq.second, level);
      break;
    case C_ASSIGN:
      indent(level);
      printf("Assign %s\n", command->attr.assign.var);
      printExpr(command->attr.assign.expr, level + 1);
      break;
    case C_IF:
      indent(level);
      printf("If\n");
      printExpr(command->attr.ifthen.cond, level + 1);
      printCmd(command->attr.ifthen.then_cmd, level + 1);
      printCmd(command->attr.ifthen.else_cmd, level + 1);
      break;
    case C_WHILE:
      indent(level);
      printf("While\n");
      printExpr(command->attr.loop.cond, level + 1);
      printCmd(command->attr.loop.body, level + 1);
      break;
    case C_WRITE:
      indent(level);
      printf("%s\n", command->attr.write.newline ? "Writeln" : "Write");
      for (args = command->attr.write.args; args; args = args->tail)
        printExpr(args->head, level + 1);
      break;
    case C_READ:
      indent(level);
      printf("%s\n", command->attr.read.newline ? "Readln" : "Read");
      printIds(command->attr.read.vars, level + 1);
      break;
  }
}

void printProgram(Program* prog) {
  Decl* d;
  printf("Program %s\n", prog->name);
  if (prog->decls) {
    indent(1);
    printf("Declaracoes\n");
    for (d = prog->decls; d; d = d->next) {
      indent(2);
      printf("Decl %s\n", d->type == INTEGER ? "integer" : "boolean");
      printIds(d->vars, 3);
    }
  }
  indent(1);
  printf("Corpo\n");
  printCmd(prog->body, 2);
}
