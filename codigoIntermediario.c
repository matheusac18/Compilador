#include "globals.h"
#include "codigoIntermediario.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void cGen(TreeNode *t);

QuadLista inicio = NULL;//lista de quadruplas

const char *OpString[] = {"add","sub","mult","divisao","maiorQue", "menorQue", "ifFalso", "call", "move","label_op"};

Endereco atual;

int loc  = 0;
int temporario = 0;
int label;

Operacao verificaOp(TokenType k)
{
	switch(k)
	{
		case SOMA:
			return add;
			break;
		case SUB:
			return sub;
			break;
		case DIV:
			return divisao;
			break;
		case MUL:
			return mult;
			break;
		case IGL:
			
			break;
		case DIF:
			
			break;
		case MAIGL:
			break;
		case MEIGL:
			
			break;
		case MAI:
			break;
		case MEN:
			break;
		default:
			break;
	}
}
/*insere um novo nó na lista de quadruplas*/
void inseriNo(Operacao op, Endereco e1, Endereco e2, Endereco e3)
{
	Quad novaQuad;

	novaQuad.op = op;
	novaQuad.end1 = e1;
	novaQuad.end2 = e2;
	novaQuad.end3 = e3;

	QuadLista novoNo = (QuadLista)malloc(sizeof(struct QuadListaNo));
  	novoNo->loc = loc;
  	novoNo->quad = novaQuad;
  	novoNo->prox = NULL;

	if(inicio == NULL)
	{
		inicio = novoNo;
	}
	else
	{
		QuadLista l = inicio;
		while(l->prox !=NULL)
		{
			l = l->prox;
		}
		l->prox = novoNo;
	}

	loc++;
}


char *criaVariavelTemp()
{
	char *temp = (char*) malloc((temporario%10+2)*sizeof(char));
	char *num = (char*) malloc(((temporario%10+1)));
	sprintf(num,"%d", temporario);
	temp[0] = 't';
	strcat(temp,num);
	temporario++;
	return temp;
}

char *criaLabel()
{
	char *temp = (char*) malloc((label%10+2)*sizeof(char));
	char *num = (char*) malloc((label%10+1)*sizeof(char));
	sprintf(num,"%d", label);
	temp[0] = 't';
	strcat(temp,num);
	label++;
	return temp;
}

/*Nós STMT: IfK,WhileK,AssignK,ReturnK, CallK, VarK,VetK,FuncaoK*/
static void genStmt(TreeNode *t)
{
	switch(t->kind.stmt)
	{
		case IfK:
			cGen(t->child[0]);//chama recursao para o argumento do if
			//inseriNo(ifFalso,)
			break;
		case AssignK:
			cGen(t->child[0]);
			Endereco e1 = atual;
			cGen(t->child[1]);
			Endereco e2 = atual;
			Endereco e3;
			e3.tipo = Vazio;
			inseriNo(move,e1,e2,e3);
			break;
		case FuncaoK:
			cGen(t->child[1]);
			break;
		default:
			break;
	}
}
/*Nós EXP: OpK,ConstK,IdK,TypeK,VarIdK,VetIdK*/
static void genExp(TreeNode *t)
{
	switch(t->kind.exp)
	{
		case OpK:
			cGen(t->child[0]);
			Endereco e1 = atual;
			cGen(t->child[1]);
			Endereco e2 = atual;
			char *dest = criaVariavelTemp();
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(dest)*sizeof(char));
			strcpy(atual.conteudo.nome,dest);
			inseriNo(verificaOp(t->attr.op),e1,e2,atual);
			break;
		case VarIdK:
			printf("Entrou1\n");
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(atual.conteudo.nome,t->attr.name);
			printf("Variavel: %s\n", atual.conteudo.nome);
			break;
		case ConstK:
			atual.tipo = Const;
			atual.conteudo.val = t->attr.val;
		case TypeK:
			cGen(t->child[0]);
			break;
	}
}

static void cGen(TreeNode *t)
{
	if(t != NULL)
	{
		switch(t->nodeKind)
		{
			case StmtK:
				genStmt(t);
				break;
			case ExpK:
				genExp(t);
				break;
			default:
				break;
		}
		cGen(t->sibling);
	}
}

void imprimeIntemediario()
{

	if(inicio == NULL)
		return;
	FILE *arq = fopen("intermediario.txt","w");

	QuadLista q = inicio;
	do
	{
		printf("entrou\n");
		fprintf(arq,"%s ",OpString[q->quad.op]);
		switch(q->quad.end1.tipo)
		{
			case Vazio:
				break;
			case Const:
				fprintf(arq, "%d ", q->quad.end1.conteudo.val);
				break;
			case String:
				fprintf(arq, "%s ", q->quad.end1.conteudo.nome);
			default:
				break;
		}
		switch(q->quad.end2.tipo)
		{
			case Vazio:
				break;
			case Const:
				fprintf(arq, "%d ", q->quad.end2.conteudo.val);
				break;
			case String:
				fprintf(arq, "%s ", q->quad.end2.conteudo.nome);
			default:
				break;
		}
		switch(q->quad.end3.tipo)
		{
			case Vazio:
				break;
			case Const:
				fprintf(arq, "%d\n", q->quad.end3.conteudo.val);
				break;
			case String:
				fprintf(arq, "%s\n", q->quad.end3.conteudo.nome);
			default:
				break;
		}
		q = q->prox;
		if(q==NULL)
			printf("é nullo\n");
	}
	while(q != NULL);
}

void gerarIntermediario(TreeNode *t)
{
	cGen(t);
	imprimeIntemediario();
}



