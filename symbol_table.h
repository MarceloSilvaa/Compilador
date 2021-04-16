#ifndef __symbol_table_h__
#define __symbol_table_h__
typedef struct _SymTable *SymTable;
typedef struct _SymTableStrings *SymTableStrings;

struct _SymTable {
		char* key;
		SymTable next;
};

struct _SymTableStrings {
		//name: .asciiz sentence
		char *name;
		char *sentence;
		SymTableStrings next;
};

SymTable mkSymTable(char* key, SymTable tail);
SymTable insert(char *key, SymTable st);
int checkKeyExists(char *key, SymTable st);

SymTableStrings mkSymTableStrings(char* name, char* sentence, SymTableStrings tail);
SymTableStrings insertString(char* name, char *sentence, SymTableStrings ss);
char* stringName(char* sentence, SymTableStrings ss); //find a string name by passing the string
#endif
