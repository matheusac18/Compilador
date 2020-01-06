#ifndef _SYMTAB_H_
#define _SYMTAB_H_

void st_insert(char *nome, char *escopo, int tipoDados, int ehvar, int lineno);

int st_lookup ( char * nome, char *escopo );

int pegaTipoFuncao(char *nome, char *escopo);

void printSymTab(FILE * listing);

#endif
