#ifndef __code_h__
#define __code_h__
#include "ast.h"
#include "symbol_table.h"

typedef enum {ATRIB, PLUSI, MINUSI, DIVI, MULTI, MODI, LABEL, GOTO, READ, PRINT, PRINTS, IFE, IFG, IFGE, IFL, IFLE, IFDIF} OpKind;

typedef enum {STRING, INTA, EMPTY} AtomKind;

typedef struct _Atom *Atom;

struct _Atom {
	AtomKind kind;
	union  {
		int value;
		char* name;
	} u;
};

typedef struct _Instr *Instr;

struct _Instr {
	OpKind op;
	Atom el1, el2, el3, el4;
};

typedef struct _List *InstrList;

struct _List{
	Instr i;
	InstrList next;
};

int comp(int v);

char* newStr();
char *newRegister();
char *newLabel();

Atom mkEmpty();
Atom mkInt(int value);
Atom mkVar(char* name);
Atom mkLabel(char* name);

Instr mkInstr(OpKind k, Atom el1, Atom el2, Atom el3, Atom el4);
InstrList mkInstrList(Instr i, InstrList tail);
Instr getFirst(InstrList l);
InstrList nextInstrs(InstrList l);
InstrList appendInstrLists(InstrList l1, InstrList l2);
void printInstr(Instr i);
void printListIntrs(InstrList il);

InstrList compileExp(Expr *e, char *r);
InstrList compileBool(BoolExpr *be, char *labelTrue, char *labelFalse, char* r);
InstrList compileInstrList(List_Cmd *cmd_list);
InstrList compileAll(List_Cmd *cmd_list);
void printMipsInstr(Instr i);
void printMipsData(SymTable st);
int isReg(char* s);
void printMips(InstrList instr_list);
#endif
