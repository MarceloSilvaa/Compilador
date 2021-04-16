#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "code.h"
#include "ast.h"
#include "parser.h"
#include "symbol_table.h"

int comp(int v) {
		switch(v) {
				case PLUS: return PLUSI;
				case SUB: return MINUSI;
				case DIV: return DIVI;
				case MOD: return MODI;
				case MULT: return MULTI;
				case EQUAL_TOKEN: return IFE;
				case NOT_EQUAL_TOKEN: return IFDIF;
				case LOWER_THAN_TOKEN: return IFL;
				case HIGHER_THAN_TOKEN: return IFG;
				case LOWEREQ_TOKEN: return IFLE;
				case HIGHEREQ_TOKEN: return IFGE;
				
		}
}

FILE* fh;
SymTableStrings ss;
int maxRegisters = 8;
int cont = 0;
int label_cont = 0;
int str_cont = 0;

char* newStr() {
	char buffer[30];
	sprintf(buffer, "strd%d", str_cont);
	str_cont = str_cont + 1;
	return strdup(buffer);
}

char* newRegister() {
	char buffer[30];
	sprintf(buffer, "t%d", cont);
	if(cont >= maxRegisters-1) 
		cont = 0;
	else
		cont = cont + 1;
	return strdup(buffer);
}

char* newLabel() {
	char buffer[30];
	sprintf(buffer, "L%d", label_cont);
	label_cont = label_cont + 1;
	return strdup(buffer);
}

Atom mkEmpty() {
		Atom a = (Atom)malloc(sizeof(*a));
		a->kind = EMPTY;
		return a;
}

Atom mkInt(int value) {
		Atom a = (Atom)malloc(sizeof(*a));
		a->kind = INTA;
		a->u.value = value;
		return a;
}

Atom mkVar(char* name) {
		Atom a = (Atom)malloc(sizeof(*a));
		a->kind = STRING;
		a->u.name = strdup(name);
		return a;
}

Atom mkLabel(char* name) {
		Atom a = (Atom)malloc(sizeof(*a));
		a->kind = STRING;
		a->u.name = strdup(name);
		return a;
}

Instr mkInstr(OpKind k, Atom el1, Atom el2, Atom el3, Atom el4) {
		Instr i = (Instr)malloc(sizeof(*i));
		i->op = k;
		i->el1 = el1;
		i->el2 = el2;
		i->el3 = el3;
		i->el4 = el4;
		return i;
}

InstrList mkInstrList(Instr i, InstrList tail) {
		InstrList il = (InstrList)malloc(sizeof(*il));
		il->i=i;
		il->next=tail;
		return il;
}

Instr getFirst(InstrList l) {
		return l->i;
}

InstrList nextInstrs(InstrList l) {
		return l->next;
}


InstrList appendInstrLists(InstrList l1, InstrList l2) {
		if (l1 == NULL) return l2;
		InstrList il = l1;
		while (il->next != NULL) {
			il = il->next;
		}
		il->next = l2;
		return l1;
}

void printInstr(Instr i) {
		switch(i->op) {
			case ATRIB:
				printf("%s=",i->el1->u.name);
				switch (i->el2->kind) {
					case INTA:
						printf("%d\n",i->el2->u.value);
					break;
					case STRING:
						printf("%s\n",i->el2->u.name);
					break;
				}
			break;
			case PLUSI:
				printf("%s=%s+%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case MINUSI:
				printf("%s=%s-%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case DIVI:
				printf("%s=%s/%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case MULTI:
				printf("%s=%s*%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case MODI:
				printf("%s=%s%%%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case LABEL:
				printf("LABEL %s:\n",i->el1->u.name);
			break;
			case GOTO:
				printf("GOTO %s\n", i->el1->u.name);
			break;
			case READ:
				printf("READ %s\n", i->el1->u.name);
			break;
			case PRINT:
				printf("PRINT %s\n", i->el1->u.name);
			break;
			case PRINTS:
				printf("PRINTS %s\n", i->el1->u.name);
				ss = insertString(newStr(),strdup(i->el1->u.name),ss);
			break;
			case IFE:
				printf("IF %s==%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
			case IFG:
				printf("IF %s>%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
			case IFGE:
				printf("IF %s>=%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
			case IFL:
				printf("IF %s<%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
			case IFLE:
				printf("IF %s<=%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
			case IFDIF:
				printf("IF %s!=%s %s %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name,i->el4->u.name);
			break;
		}
}

void printListIntrs(InstrList il) {
		while (il != NULL) {
			printInstr(il->i);
			il = il->next;
		}
}

InstrList compileExp(Expr* e, char *r) {
		char *r1;
		char *r2;
		InstrList l;
		switch(e->kind) {
				case E_INTEGER:
					l = mkInstrList(mkInstr(ATRIB,mkVar(r),mkInt(e->attr.value),mkEmpty(),mkEmpty()), NULL);
					return l;
				break;
				case E_OPERATION:
					r1 = strdup(newRegister());
					r2 = strdup(newRegister());
					InstrList l1 = compileExp(e->attr.op.left, r1);
					InstrList l2 = compileExp(e->attr.op.right, r2);
					InstrList l3 = appendInstrLists(l1,l2);
					InstrList l4 = appendInstrLists(l3,mkInstrList(mkInstr(comp(e->attr.op.operator),mkVar(r),mkVar(r1),mkVar(r2),mkEmpty()), NULL));
					return l4;
				break;
				case E_VARIABLE:
					l = mkInstrList(mkInstr(ATRIB,mkVar(r),mkVar(e->attr.variable_name),mkEmpty(),mkEmpty()), NULL);
					return l;
				break;
		}
}

InstrList compileBool(BoolExpr *be, char *labelTrue, char* labelFalse, char* r) {
	InstrList l1;
	InstrList l2;
	InstrList l3;
	char* r1;
	char* r2;
	switch(be->kind) {
		case E_BOOL:
			return mkInstrList(mkInstr(ATRIB, mkVar(r), mkInt(be->attr.boolean_value), mkEmpty(), mkEmpty()), NULL);
		break;
		case E_OP:
			r1 = strdup(newRegister());
			r2 = strdup(newRegister());
			l1 = compileExp(be->attr.op.left, r1); //compile left expression
			l2 = compileExp(be->attr.op.right, r2); //compile right expression
			l3 = mkInstrList(mkInstr(comp(be->attr.op.relOp), mkVar(r1), mkVar(r2), mkLabel(labelTrue), mkLabel(labelFalse)), NULL); //Create the if instruction
			return appendInstrLists(l1,appendInstrLists(l2,l3));
		break;
		case E_BOOL_COMP: //0 = false, 1 = true
			r1 = strdup(newRegister());
			r2 = strdup(newRegister());
			l1 = compileBool(be->attr.op.bleft, labelTrue, labelFalse, r1);
			l2 = compileBool(be->attr.op.bright, labelTrue, labelFalse, r2);
			l3 = mkInstrList(mkInstr(comp(be->attr.op.relOp), mkVar(r1), mkVar(r2), mkLabel(labelTrue), mkLabel(labelFalse)), NULL);
			return appendInstrLists(l1,appendInstrLists(l2,l3));
		break;
	}
}

InstrList compileAll(List_Cmd *cmd_list) {
		InstrList l = compileInstrList(cmd_list);
		if (cmd_list->next != NULL) {
				l = appendInstrLists(l, compileAll(cmd_list->next));
		}
		return l;
}

InstrList compileInstrList(List_Cmd *cmd_list) {
	char* labelT;
	char* labelF;
	char* labelFim;
	char* labelInicio;
	char* r;
	char* fullString;
	char* token;
	char* string1;
	char* string2;
	InstrList l;
	InstrList code1;
	InstrList code2;
	InstrList code3;
	Instr labelTinstr;
	InstrList labelTlist;
	Instr labelFinstr;
	InstrList labelFlist;
	Instr labelFimInstr;
	InstrList labelFimlist;
	Instr labelInicioInstr;
	InstrList labelIniciolist;
	Instr goToInstr;
	InstrList goToInstrList;
	Instr readInstr;
	InstrList readInstrList;
	Instr printStrInstr;
	InstrList printStrInstrList;
	Instr printVarInstr;
	InstrList printVarInstrList;
	Instr printString1Instr;
	InstrList printString1InstrList;
	Instr printString2Instr;
	InstrList printString2InstrList;
	Instr printLineChangeInstr;
	InstrList printLineChangeInstrList;
	
	switch(cmd_list->elem->kind) {
		case C_LET:
		  r = strdup(newRegister());
		  l = compileExp(cmd_list->elem->attr.variable_data.value, r);
		  return appendInstrLists(l, mkInstrList(
			mkInstr(ATRIB,mkVar(cmd_list->elem->attr.variable_data.variable_name),mkVar(r),mkEmpty(),mkEmpty()),
			NULL
		  ));
		  break;
		case C_IF:
		  labelT = strdup(newLabel());
		  labelF = strdup(newLabel());
		  code1 = compileBool(cmd_list->elem->attr.if_data.bool_expr, labelT, labelF, "");
		  code2 = compileAll(cmd_list->elem->attr.if_data.cmd_list);
		  labelTinstr = mkInstr(LABEL, mkLabel(labelT), mkEmpty(), mkEmpty(), mkEmpty());
		  labelFinstr = mkInstr(LABEL, mkLabel(labelF), mkEmpty(), mkEmpty(), mkEmpty());
		  labelTlist = mkInstrList(labelTinstr, NULL);
		  labelFlist = mkInstrList(labelFinstr, NULL);
		  return appendInstrLists(code1,
								  appendInstrLists(labelTlist,
								  appendInstrLists(code2, labelFlist)
								  )
		  );
		  break;
		case C_IFELSE:
		  labelT = strdup(newLabel());
		  labelF = strdup(newLabel());
		  labelFim = strdup(newLabel());
		  code1 = compileBool(cmd_list->elem->attr.ifelse_data.bool_expr, labelT, labelF, "");
		  code2 = compileAll(cmd_list->elem->attr.ifelse_data.cmd_listIF);
		  code3 = compileAll(cmd_list->elem->attr.ifelse_data.cmd_listELSE);
		  goToInstr = mkInstr(GOTO, mkLabel(labelFim), mkEmpty(), mkEmpty(), mkEmpty());
		  labelTinstr = mkInstr(LABEL, mkLabel(labelT), mkEmpty(), mkEmpty(), mkEmpty());
		  labelFinstr = mkInstr(LABEL, mkLabel(labelF), mkEmpty(), mkEmpty(), mkEmpty());
		  labelFimInstr = mkInstr(LABEL, mkLabel(labelFim), mkEmpty(), mkEmpty(), mkEmpty());
		  labelTlist = mkInstrList(labelTinstr, NULL);
		  labelFlist = mkInstrList(labelFinstr, NULL);
		  labelFimlist = mkInstrList(labelFimInstr, NULL);
		  goToInstrList = mkInstrList(goToInstr, NULL);
		  return appendInstrLists(code1,
								  appendInstrLists(labelTlist,
								  appendInstrLists(code2,
								  appendInstrLists(goToInstrList,
								  appendInstrLists(labelFlist,
								  appendInstrLists(code3, labelFimlist)
								  )
		  ))));
		  break;
		case C_WHILE:
			labelInicio = strdup(newLabel());
			labelT = strdup(newLabel());
			labelF = strdup(newLabel());
			code1 = compileBool(cmd_list->elem->attr.while_data.bool_expr,labelT,labelF,"");
			code2 = compileAll(cmd_list->elem->attr.while_data.cmd_list);
			goToInstr = mkInstr(GOTO, mkLabel(labelInicio), mkEmpty(), mkEmpty(), mkEmpty());
			labelTinstr = mkInstr(LABEL, mkLabel(labelT), mkEmpty(), mkEmpty(), mkEmpty());
		    labelFinstr = mkInstr(LABEL, mkLabel(labelF), mkEmpty(), mkEmpty(), mkEmpty());
			labelInicioInstr = mkInstr(LABEL, mkLabel(labelInicio), mkEmpty(), mkEmpty(), mkEmpty());
			labelTlist = mkInstrList(labelTinstr, NULL);
		    labelFlist = mkInstrList(labelFinstr, NULL);
			labelIniciolist = mkInstrList(labelInicioInstr, NULL);
			goToInstrList = mkInstrList(goToInstr, NULL);
			return appendInstrLists(labelIniciolist,
									appendInstrLists(code1,
									appendInstrLists(labelTlist,
									appendInstrLists(code2,
									appendInstrLists(goToInstrList, labelFlist
									)
			))));
		  break;
		case C_READLINE:
			readInstr = mkInstr(READ, mkVar(cmd_list->elem->attr.readline_data.variable_name), mkEmpty(), mkEmpty(), mkEmpty());
			readInstrList = mkInstrList(readInstr, NULL);
			return readInstrList;
		  break;
		case C_PRINTLN:
			//Print string
			printStrInstr = mkInstr(PRINTS, mkLabel(cmd_list->elem->attr.println_data.p_value), mkEmpty(), mkEmpty(), mkEmpty());
			printStrInstrList = mkInstrList(printStrInstr, NULL);
			
			printLineChangeInstr = mkInstr(PRINTS, mkLabel("\"\\n\""), mkEmpty(), mkEmpty(), mkEmpty());
			printLineChangeInstrList = mkInstrList(printLineChangeInstr, NULL);
			
			return appendInstrLists(printStrInstrList,printLineChangeInstrList);
		  break;
		 case C_PRINTLNV:
			//Print variable + String(if exists)
			printVarInstr = mkInstr(PRINT, mkVar(cmd_list->elem->attr.println_var.var_name), mkEmpty(), mkEmpty(), mkEmpty());
			printVarInstrList = mkInstrList(printVarInstr, NULL);
			
			printLineChangeInstr = mkInstr(PRINTS, mkLabel("\"\\n\""), mkEmpty(), mkEmpty(), mkEmpty());
			printLineChangeInstrList = mkInstrList(printLineChangeInstr, NULL);
			
			fullString = strdup(cmd_list->elem->attr.println_var.string_val);

			//String doesn't have any %d
			if(strstr(fullString, "%d") == NULL) {
				if(strcmp(fullString,"\"\"")==0) {
					//println!("",x);
					return appendInstrLists(printVarInstrList,printLineChangeInstrList);
				}
				else {
					//println!("asdasd",x);
					printStrInstr = mkInstr(PRINTS, mkLabel(fullString), mkEmpty(), mkEmpty(), mkEmpty());
					printStrInstrList = mkInstrList(printStrInstr, NULL);
					return appendInstrLists(printStrInstrList,appendInstrLists(printVarInstrList,printLineChangeInstrList));
				}
			}
			
			token = strtok(fullString, "%d");
			
			//Get first part of the string
			if(token != NULL) {
				string1 = strdup(token);
				strcat(string1,"\"");
				token = strtok(NULL, "%d");
			}
			
			//Get second part of the string
			if(token != NULL) {
				string2 = strdup("\"");
				strcat(string2,strdup(token));
			}

			//String has %d
			if(strcmp(string1,"\"\"") == 0) {
				if(strcmp(string2,"\"\"") == 0) {
					//String of type: %d
					return appendInstrLists(printVarInstrList,printLineChangeInstrList);
				}
				else {
					//String of type: %d zxc
					printString2Instr = mkInstr(PRINTS, mkLabel(string2), mkEmpty(), mkEmpty(), mkEmpty());
					printString2InstrList = mkInstrList(printString2Instr, NULL);
					return appendInstrLists(printVarInstrList,appendInstrLists(printString2InstrList,printLineChangeInstrList));
				}
			}
			
			else {
				if(strcmp(string2,"\"\"") == 0) {
					//String of type: zxc %d
					printString1Instr = mkInstr(PRINTS, mkLabel(string1), mkEmpty(), mkEmpty(), mkEmpty());
					printString1InstrList = mkInstrList(printString1Instr, NULL);
					return appendInstrLists(printString1InstrList,appendInstrLists(printVarInstrList,printLineChangeInstrList));
				}
				else {
					//String of type: zxc %d zxc
					printString1Instr = mkInstr(PRINTS, mkLabel(string1), mkEmpty(), mkEmpty(), mkEmpty());
					printString1InstrList = mkInstrList(printString1Instr, NULL);
					
					printString2Instr = mkInstr(PRINTS, mkLabel(string2), mkEmpty(), mkEmpty(), mkEmpty());
					printString2InstrList = mkInstrList(printString2Instr, NULL);
					
					return appendInstrLists(printString1InstrList,appendInstrLists(printVarInstrList,
									appendInstrLists(printString2InstrList,printLineChangeInstrList)));
				}
			}
			break;
	}
}

void printMipsData(SymTable st) {
	while(st != NULL) {
			fprintf(fh,"%s: .space 4\n",st->key);
			st=st->next;
	}
	SymTableStrings ss_aux = ss;
	while (ss_aux != NULL) {
		fprintf(fh,"%s: .asciiz %s\n",ss_aux->name,ss_aux->sentence);
		ss_aux=ss_aux->next;
	}
}

int isReg(char* s) {
	return (strlen(s) == 2 && s[0] == 't' && isdigit(s[1]));
}

void printMips(InstrList il) {
	fh = fopen("code.asm","w"); //Using w it erases the contents and writes new content on file. Also creates if it doesn't exist.
	fprintf(fh, ".data\n");
	printMipsData(st);
	fprintf(fh,"\n");
	fprintf(fh,".text\n");
	while (il != NULL) {
			printMipsInstr(il->i);
			il = il->next;
	}
	//Exit program properly
	fprintf(fh,"li $v0, 10\n");
	fprintf(fh,"syscall\n");
}

void printMipsInstr(Instr i) {
	switch(i->op) {
			case ATRIB:
				switch (i->el2->kind) {
					case INTA:
						fprintf(fh,"li $%s, %d\n",i->el1->u.name,i->el2->u.value);
					break;
					case STRING:
						if (isReg(i->el1->u.name)) {
								fprintf(fh,"lw $%s, %s\n",i->el1->u.name,i->el2->u.name);
						}
						else {
							fprintf(fh,"sw $%s, %s\n",i->el2->u.name,i->el1->u.name); //assign the value to the variable
						}
					break;
				}
			break;
			case PLUSI:
				fprintf(fh,"add $%s, $%s, $%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case MINUSI:
				fprintf(fh,"sub $%s, $%s, $%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case DIVI:
				fprintf(fh,"div $%s, $%s\n",i->el2->u.name,i->el3->u.name);
				fprintf(fh,"mflo $%s\n",i->el1->u.name); //move to the register the quotient of the division
			break;
			case MULTI:
				fprintf(fh,"mul $%s, $%s, $%s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
			break;
			case MODI:
				fprintf(fh,"div $%s, $%s\n",i->el2->u.name,i->el3->u.name);
				fprintf(fh,"mfhi $%s\n",i->el1->u.name); //move to the register the remainder of the division
			break;
			case LABEL:
				fprintf(fh,"%s:\n",i->el1->u.name);
			break;
			case GOTO:
				fprintf(fh,"b %s\n", i->el1->u.name);
			break;
			case READ:
			   fprintf(fh,"li $v0, 5\n");
			   fprintf(fh,"syscall\n");
			   //The read value is now at $v0
			   fprintf(fh,"sw $v0, %s\n",i->el1->u.name);
			break;
			case PRINT:
			   fprintf(fh,"lw $a0, %s\n",i->el1->u.name);
			   fprintf(fh,"li $v0, 1\n");
			   fprintf(fh,"syscall\n");
			break;
			case PRINTS:
			   fprintf(fh,"la $a0, %s\n",stringName(i->el1->u.name, ss));
			   fprintf(fh,"li $v0, 4\n");
			   fprintf(fh,"syscall\n");
			break;
			case IFE:
				fprintf(fh,"beq $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
			case IFG:
				fprintf(fh,"bgt $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
			case IFGE:
				fprintf(fh,"bge $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
			case IFL:
				fprintf(fh,"blt $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
			case IFLE:
				fprintf(fh,"ble $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
			case IFDIF:
				fprintf(fh,"bne $%s, $%s, %s\n",i->el1->u.name,i->el2->u.name,i->el3->u.name);
				fprintf(fh,"b %s\n",i->el4->u.name);
			break;
		}
}

