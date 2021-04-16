
// AST definitions
#ifndef __ast_h__
#define __ast_h__


typedef struct _Expr Expr; // Convenience typedef
typedef struct _BoolExpr BoolExpr;
typedef struct _Cmd Cmd;
typedef struct _List_Cmd List_Cmd;
// AST for expressions
struct _Expr {
  enum { 
    E_INTEGER,
    E_OPERATION,
    E_VARIABLE
  } kind;
  union {
    int value; // for integer values
    char* variable_name;
    struct { 
      int operator; // PLUS, MINUS, etc 
      struct _Expr* left;
      struct _Expr* right;
    } op; // for binary expressions
  } attr;
};

struct _BoolExpr {
  enum {
    E_BOOL, //If it is only a boolean value (true or false)
    E_OP, //If it is an operation between expressions (1+1==3+3)
    E_BOOL_COMP //If it is an operation between boolean values (true==false)
  } kind;
  union {
    int boolean_value;
    struct {
      int relOp; //'==', '!=', '<', '>', '<=', '>='
      struct _Expr* left;
      struct _Expr* right;
      struct _BoolExpr* bleft;
      struct _BoolExpr* bright;
    } op;
  } attr;
};

struct _Cmd {
  enum {
    C_LET, //let, used for variables
    C_IF, //if, used for if statements
    C_IFELSE, //if with else
    C_WHILE, //while, used for while loops
    C_PRINTLN, //print only string
    C_PRINTLNV, //Print string with variable
    C_READLINE
  } kind;
  union {
    struct { //If it is let, need to know the variable name and value
      char* variable_name;
      Expr* value;
    } variable_data;
    struct { //If it is the while command, need to know the condition and commands
      BoolExpr* bool_expr;
      List_Cmd* cmd_list;
    } while_data;
    struct { //If it is the readline, need to know the variable name where to store
      char* variable_name;
    } readline_data;
    struct { // If it is the if, need to know the expression and commands
      BoolExpr* bool_expr;
      List_Cmd* cmd_list;
    } if_data;
    struct {
	  BoolExpr* bool_expr;
      List_Cmd* cmd_listIF;	
      List_Cmd* cmd_listELSE;
    } ifelse_data;
    struct {
      char* p_value;
    } println_data; //Only string
    struct {
	  char* string_val;
	  char* var_name;
	} println_var; //String with variable
  } attr;
};




//Struct to save list of commands
struct _List_Cmd {
  Cmd *elem;
  List_Cmd *next;
};

// Constructor functions (see implementation in ast.c)
Expr* ast_integer(int v);
Expr* ast_operation(int operator, Expr* left, Expr* right);
Expr* ast_variable(char* variable_name);

BoolExpr* ast_bool(int boolean_value);
BoolExpr* ast_bop(int op, Expr* left, Expr* right);
BoolExpr* ast_bop_neq(int op, BoolExpr* left, BoolExpr* right);

List_Cmd* ast_cmd_list(Cmd* elem, List_Cmd* cmd_list);

Cmd* ast_cmd_var(char* variable_name, Expr* value);
Cmd* ast_cmd_while(BoolExpr* bool_expr, List_Cmd* cmd_list);
Cmd* ast_cmd_readline(char* variable_name);
Cmd* ast_cmd_if(BoolExpr* bool_expr, List_Cmd* cmd_list);
Cmd* ast_cmd_if_with_else(BoolExpr* bool_expr, List_Cmd* cmd_list1, List_Cmd* cmd_list2);
Cmd* ast_cmd_printlnSTR(char* p_value);
Cmd* ast_cmd_printlnVAR(char* string_val, char* var_name);
#endif
