#include "globals.h"
#include "analyze.h"
#include "stdio.h"
#include "symtab.h"
#include "util.h"

FILE* symTabFile;


static void insertNode(TreeNode * t)
{
	int tipoFuncao = 0;
    switch (t->nodeKind)
    {
    case StmtK:
        switch (t->kind.stmt)
        {
        case VarK://se o nó for declaração de variavel

            if(!st_lookup(t->attr.name,t->escopo) && !st_lookup(t->attr.name,"global") )//não foi encontrada na tabela de simbolos
            {
                if(t->type == Void)  // erro 3 e erro 7
                {
                    printf("ERRO SEMÂNTICO: Variáveis não podem ser do tipo Void linha: %d\n",t->lineno);
                    break;
                }
                st_insert(t->attr.name, t->escopo, t->type, 1, t->lineno);
            }
            else //já está na tabela de simbolos //erro 4
            {
                printf("ERRO SEMÂNTICO: Redeclaração da variavel '%s' linha: %d\n",t->attr.name,t->lineno);
            }

            break;

        case VetK://se o no for declaração de vetor
            if(!st_lookup(t->attr.name,t->escopo) && !st_lookup(t->attr.name,"global"))
            {
                if(t->type == Void)  // erro 3 e erro 7
                {
                    printf("ERRO SEMÂNTICO: Variáveis não podem ser do tipo Void linha: %d\n",t->lineno);
                    break;
                }
                st_insert(t->attr.name, t->escopo, t->type, 1, t->lineno);
            }
            else //erro 4
                printf("ERRO SEMÂNTICO: Redeclaração do vetor '%s' linha: %d\n",t->attr.name,t->lineno);
            break;
        case FuncaoK:
            if (!st_lookup(t->attr.name,"global"))
            {
                st_insert(t->attr.name, "global", t->type, 0, t->lineno);
            }
            else
                printf("ERRO SEMÂNTICO: Redeclaração da função '%s' linha: %d\n",t->attr.name,t->lineno);
            break;
        case CallK: //erro 5
            if (!st_lookup(t->attr.name,"global") && strcmp(t->attr.name,"input") != 0 && strcmp(t->attr.name,"output") != 0)//chamada de funcao não encontrada
                printf("ERRO SEMÂNTICO: Chamada da função '%s' não declarada linha: %d\n",t->attr.name,t->lineno);
            else if (strcmp(t->attr.name,"input") != 0 && strcmp(t->attr.name,"output") != 0)
                st_insert(t->attr.name, "global", 0, 0, t->lineno);
            break;
        case AssignK:
			if(t->child[1]->kind.stmt == CallK && t->child[1]->nodeKind == 0)//está atribuindo o retorno de uma função à varivel
			{
				tipoFuncao = pegaTipoFuncao(t->child[1]->attr.name, "global");
				if(tipoFuncao != -1)
				{
					if(t->child[0]->type != tipoFuncao)
            		{
                		printf("ERRO SEMÂNTICO: Atribuição de tipo invalido para a variavel %s na linha: %d\n",t->child[0]->attr.name,t->child[0]->lineno);
            		}
				}
			}

			else if(t->child[0]->type != t->child[1]->type)
            {
                printf("ERRO SEMÂNTICO: Atribuição de tipo invalido para a variavel %s na linha: %d\n",t->child[0]->attr.name,t->child[0]->lineno);
            }
        default:
            break;
        }
        break;
    case ExpK:

        switch (t->kind.exp)
        {
        case VarIdK:
            if(st_lookup(t->attr.name,"global"))
            {
                st_insert(t->attr.name,"global",0,0,t->lineno);
                t->escopo = copyString("global");//se encontrou o VarIdK  no escopo global altera seu escopo para global
            }
            else if (st_lookup(t->attr.name,t->escopo)) // erro 1
                st_insert(t->attr.name,t->escopo,0,0,t->lineno);
            else
                printf("ERRO SEMÂNTICO: Variavel '%s' não declarada na linha: %d\n", t->attr.name,t->lineno);
            break;
        case VetIdK:
            if (st_lookup(t->attr.name,t->escopo))
                st_insert(t->attr.name,t->escopo,0,0,t->lineno);
            else if (st_lookup(t->attr.name,"global")) // erro 1
            {
                st_insert(t->attr.name,"global",0,0,t->lineno);
                t->escopo = copyString("global");//se encontrou o VetIdK  no escopo global altera seu escopo para global
            }
            else
                printf("ERRO SEMÂNTICO: Vetor '%s' não declarado na linha: %d\n", t->attr.name,t->lineno);
            break;
        default:

            break;
        }
        break;
    default:
        break;
    }
}

static void verifica_main()
{
    if (!st_lookup("main","global")) // erro 1
        printf("ERRO SEMÂNTICO: Função main() não declarada\n");
}


static void traverse(TreeNode *t)
{
    if(t!=NULL)
    {
        insertNode(t);
        int i;
        for(i=0; i<MAXCHILDREN; i++)
            traverse(t->child[i]);
        traverse(t->sibling);
    }
}

void buildSymtab(TreeNode *syntaxTree)
{
    traverse(syntaxTree);
    verifica_main();
    symTabFile = fopen("tabelaSimbolos.txt","w");
    if(symTabFile!=NULL)
    {
        printSymTab(symTabFile);
    }
}
