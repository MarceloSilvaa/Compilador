#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"


SymTable mkSymTable(char* key, SymTable tail) {
		SymTable st = malloc(sizeof(*st));
		st->key = strdup(key);
		st->next = tail;
		return st;
}

SymTable insert(char *key, SymTable st) {
		//Verify that the key doesn't exist
		if(checkKeyExists(key,st)) { //The key is already in the table
				return st;
		}
		//Insert the value
		return mkSymTable(strdup(key),st);
}

int checkKeyExists(char *key, SymTable st) {
	SymTable st_aux = st;
	while (st_aux != NULL) {
		if (!strcmp(key,st_aux->key)) return 1; //The key is already on the table
		st_aux=st_aux->next;
	}
	return 0;
}

SymTableStrings mkSymTableStrings(char* name, char* sentence, SymTableStrings tail) {
		SymTableStrings ss = malloc(sizeof(*ss));
		ss->name = strdup(name);
		ss->sentence = strdup(sentence);
		ss->next = tail;
		return ss;
}

SymTableStrings insertString(char* name, char *sentence, SymTableStrings ss) {
		//Insert the value
		return mkSymTableStrings(strdup(name),strdup(sentence),ss);
}

char* stringName(char* sentence, SymTableStrings ss) {
		SymTableStrings ss_aux = ss;
		while (ss_aux != NULL) {
				if (!strcmp(sentence,ss_aux->sentence)) return strdup(ss_aux->name);
				ss_aux=ss_aux->next;
		}
		return "";
}
