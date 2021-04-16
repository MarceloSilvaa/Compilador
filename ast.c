// AST constructor functions

#include <stdlib.h> // for malloc
#include "ast.h" // AST header
#include <string.h>

Expr* ast_integer(int v) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_INTEGER;
  node->attr.value = v;
  return node;
}

Expr* ast_operation(int operator, Expr* left, Expr* right) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_OPERATION;
  node->attr.op.operator = operator;
  node->attr.op.left = left;
  node->attr.op.right = right;
  return node;
}

Expr* ast_variable(char* variable_name) {
  Expr* node = (Expr*) malloc(sizeof(Expr));
  node->kind = E_VARIABLE;
  node->attr.variable_name = strdup(variable_name);
  return node;
}

BoolExpr* ast_bool(int boolean_value) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = E_BOOL;
  node->attr.boolean_value = boolean_value;
  return node;
}

BoolExpr* ast_bop(int op, Expr* left, Expr* right) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = E_OP;
  node->attr.op.relOp = op;
  node->attr.op.left = left;
  node->attr.op.right = right;
  return node;
}

BoolExpr* ast_bop_neq(int op, BoolExpr* left, BoolExpr* right) {
  BoolExpr* node = (BoolExpr*) malloc(sizeof(BoolExpr));
  node->kind = E_BOOL_COMP;
  node->attr.op.relOp = op;
  node->attr.op.bleft = left;
  node->attr.op.bright = right;
  return node;
}

List_Cmd* ast_cmd_list(Cmd* elem, List_Cmd* cmd_list) {
  List_Cmd* node = (List_Cmd*) malloc(sizeof(List_Cmd));
  node->elem = elem;
  node->next = cmd_list;
  return node;
}

Cmd* ast_cmd_var(char* variable_name, Expr* value) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_LET;
  node->attr.variable_data.variable_name = variable_name;
  node->attr.variable_data.value = value;
  return node;
}

Cmd* ast_cmd_while(BoolExpr* bool_expr, List_Cmd* cmd_list) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_WHILE;
  node->attr.while_data.bool_expr = bool_expr;
  node->attr.while_data.cmd_list = cmd_list;
  return node;
}

Cmd* ast_cmd_readline(char* variable_name) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_READLINE;
  node->attr.readline_data.variable_name = strdup(variable_name);
  return node;
}

Cmd* ast_cmd_if(BoolExpr* bool_expr, List_Cmd* cmd_list) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_IF;
  node->attr.if_data.bool_expr = bool_expr;
  node->attr.if_data.cmd_list = cmd_list;
  return node;
}

Cmd* ast_cmd_if_with_else(BoolExpr* bool_expr, List_Cmd* cmd_list1, List_Cmd* cmd_list2) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_IFELSE;
  node->attr.ifelse_data.bool_expr = bool_expr;
  node->attr.ifelse_data.cmd_listIF = cmd_list1;
  node->attr.ifelse_data.cmd_listELSE = cmd_list2;
  return node;
}

Cmd* ast_cmd_printlnSTR(char* p_value) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_PRINTLN;
  node->attr.println_data.p_value = strdup(p_value);
  return node;
}

Cmd* ast_cmd_printlnVAR(char* string_val, char* var_name) {
  Cmd* node = (Cmd*) malloc(sizeof(Cmd));
  node->kind = C_PRINTLNV;
  node->attr.println_var.string_val = strdup(string_val);
  node->attr.println_var.var_name = strdup(var_name);
  return node;
}
