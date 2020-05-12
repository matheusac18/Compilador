#ifndef _GLOBALS_H_
#define _GLOBALS_H_


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
#include "parser.tab.h"
#endif



typedef int TokenType;

extern int lineno; /* contador de linhas */

extern FILE* source; /*arquivo de entrada para o compilador*/



/*************************************************************/
/********** Arvore sintática para análise sintática **********/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,WhileK,AssignK,ReturnK, CallK, VarK,VetK,FuncaoK} StmtKind;
typedef enum {OpK,ConstK,IdK,TypeK,VarIdK,VetIdK} ExpKind;

typedef enum {Void, Integer, Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode{
	struct treeNode *child[MAXCHILDREN];
	struct treeNode *sibling;

	int lineno;
    char *escopo;
	NodeKind nodeKind;
	union {StmtKind stmt; ExpKind exp;} kind;
	union {TokenType op; int val; char * name;}attr;

	ExpType type;

}TreeNode;

#endif
