#include "globals.h"
#include "util.h"
static int indentno = 0;
#define INDENT indentno+=2
#define UNINDENT indentno-=2

FILE* listing;

/*Função new StmtNode cria um novo nó de declaraçõ para a construção da arvore sintática*/

TreeNode *newStmtNode(StmtKind kind)
{
	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
	int i;
	if(t!=NULL)
	{
		//printf("Entrou1\n" );
		for(i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodeKind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
	}
	return t;
}

/*Função newExpNode cria um novo nó de expressao para a construção da arvore sintática*/
TreeNode *newExpNode(ExpKind kind)
{
	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
	int i;
	if(t!=NULL)
	{	
		for(i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodeKind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;
		//printf("Entrou exp node %d aqui \n",t->nodeKind);
	}
	return t;
}

void insereEscopo(TreeNode *tree, char *escopo)
{
    int i;

  	while (tree != NULL) {
  		tree->escopo = copyString(escopo);
	    for (i=0;i<MAXCHILDREN;i++)
	   	{
	   		if(tree->child[i]!=NULL)
	   		{
	         	insereEscopo(tree->child[i],escopo);
	   		}
	   	}
    	tree = tree->sibling;
  	}
}

char *copyString(char *s)
{
	int n;
	char *t;
	if(s==NULL) return NULL;
	n = strlen(s)+1;
	t = malloc(n);
	if(t!=NULL){
		strcpy(t,s);
	}
	return t;
}




char *printToken(TokenType token, const char* tokenString)
{
	char *tokenS;
	tokenS = malloc(100);
	switch(token)
	{
		case IF:
		case ELSE:
		case INT:
		case VOID:
		case WHILE:
		case RETURN:
			strcpy(tokenS,tokenString);
			break;
		case NUM:
			strcpy(tokenS,"NUM");
			break;
		case SOMA:
			strcpy(tokenS,"+");
			break;
		case SUB:
			strcpy(tokenS,"-");
			break;
		case DIV:
			strcpy(tokenS,"/");
			break;
		case MUL:
			strcpy(tokenS,"*");
			break;
		case APR:
			strcpy(tokenS,"(");
			break;
		case FPR:
			strcpy(tokenS,")");
			break;
		case ID:
			strcpy(tokenS,tokenString);
			break;
		case ACOL:
			strcpy(tokenS,"[");
			break;
		case FCOL:
			strcpy(tokenS,"]");
			break;
		case ACH:
			strcpy(tokenS,"{");
			break;
		case FCH:
			strcpy(tokenS,"}");
			break;
		case ATRIB:
			strcpy(tokenS,"=");
			break;
		case IGL:
			strcpy(tokenS,"==");
			break;
		case DIF:
			strcpy(tokenS,"!=");
			break;
		case MAIGL:
			strcpy(tokenS,">=");
			break;
		case MEIGL:
			strcpy(tokenS,"<=");
			break;
		case MAI:
			strcpy(tokenS,">");
			break;
		case MEN:
			strcpy(tokenS,"<");
			break;
		case VIRG:
			strcpy(tokenS,",");
			break;
		case PV:
			strcpy(tokenS,";");
			break;
	}
	return tokenS;
}

static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

char *getType(int x)
{
	char *tipo;
	if(x)
	{
		tipo = (char*) malloc(4*sizeof(char));
		strcpy(tipo,"int");
	}
	else if(x==0)
	{
		tipo = (char*) malloc(5*sizeof(char));
		strcpy(tipo,"void");
	}
	return tipo;
}

void printTree(TreeNode * tree )
{ 
	int i;
    INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodeKind==StmtK)
    { 
	    switch (tree->kind.stmt) {
	        case IfK:
	          fprintf(listing,"If\n");
	          break;
	        case WhileK:
	          fprintf(listing,"while\n");
	          break;
	        case AssignK:
	          fprintf(listing,"Assign\n");
	          break;
	        case ReturnK:
	          fprintf(listing,"Return\n");
	          break;
	        case CallK:
	          fprintf(listing,"Call: %s Linha: %d\n",tree->attr.name,tree->lineno);
	          break;
			case FuncaoK:
		      fprintf(listing,"Funcão - %s - %s - Linha: %d Escopo: %s\n",getType(tree->type),tree->attr.name,tree->lineno,tree->escopo);
		      break;
   	   		case VarK:
          	  fprintf(listing,"Var %s Linha: %d Escopo: %s\n",tree->attr.name,tree->lineno,tree->escopo);
              break;
			case VetK:
          	  fprintf(listing,"Vetor %s Linha: %d Escopo: %s\n",tree->attr.name,tree->lineno,tree->escopo);
			  break;
	        default:
	          fprintf(listing,"Unknown ExpNode kind\n");
	          break;
	      }
    }
    else if (tree->nodeKind==ExpK)
    { 

    	switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: %s\n",printToken(tree->attr.op,"\0"));
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Id: %s Line:%d\n",tree->attr.name,tree->lineno);
          break;
        case TypeK:
          fprintf(listing,"Tipo: %s\n",getType(tree->type));
          break;
        case VetIdK:
          	fprintf(listing,"VetID: %s Line: %d Escopo: %s\n",tree->attr.name,tree->lineno,tree->escopo);
          	break;
		case VarIdK:
			fprintf(listing,"VarID: %s Line: %d Escopo: %s\n",tree->attr.name, tree->lineno,tree->escopo);
			break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
   	{
   		if(tree->child[i]!=NULL)
   		{
         	printTree(tree->child[i]);
   		}
   	}
	
    tree = tree->sibling;

  }
UNINDENT;
}

void printTreeR( TreeNode * tree )
{
	listing = fopen("arvore.txt","w");
	if(listing!=NULL)
	{
		printTree(tree);
	}
	
}
