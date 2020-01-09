#include "globals.h"
#include "codigoIntermediario.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void cGen(TreeNode *t);

QuadLista inicio = NULL;//lista de quadruplas

const char *OpString[] = {"add","sub","mult","divisao","maiorQue", "menorQue", "ifFalso", "call", "move","label_op","storeIm","jump","slt","sgt","slet","sget","set","sdt","funInicio","funFim", "load","store","allocaMemVar","allocaMemVet"};

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
			return set;	
			break;
		case DIF:
			return sdt;
			break;
		case MAIGL:
			return sget;
			break;
		case MEIGL:
			return slet;
			break;
		case MAI:
			return sgt;
			break;
		case MEN:
			return slt;
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
	temp[0] = 'l';
	strcat(temp,num);
	label++;
	return temp;
}

/*Nós STMT: WhileK,ReturnK, CallK, VarK,VetK*/
static void genStmt(TreeNode *t)
{
	Endereco e1;
	Endereco e2;
	Endereco e3;
	switch(t->kind.stmt)
	{
		case IfK:
			cGen(t->child[0]);//chama recursao para o argumento do if
			char *destLabel = criaLabel();
			e1 = atual;

			Endereco condFalsa;
			condFalsa.tipo = String;
			condFalsa.conteudo.nome = (char*) malloc(strlen(destLabel)*sizeof(char));
			strcpy(condFalsa.conteudo.nome,destLabel);	

			e2.tipo = Vazio;

			inseriNo(ifFalso,e1,condFalsa,e2);

			cGen(t->child[1]);

			if(t->child[2] == NULL)
			{
				e1.tipo = Vazio;
				e2.tipo = Vazio;
				inseriNo(label_op,condFalsa,e1,e2);//label,vazio,vazio
			}
			else
			{
				destLabel = criaLabel();
				Endereco fimIf;
				fimIf.tipo = String;
				fimIf.conteudo.nome = (char*) malloc(strlen(destLabel)*sizeof(char));
				strcpy(fimIf.conteudo.nome,destLabel);	
				e1.tipo = Vazio;
				e3.tipo = Vazio;
				inseriNo(jump,fimIf,e1,e2);
				inseriNo(label_op,condFalsa,e1,e2);
				cGen(t->child[2]);
				inseriNo(label_op,fimIf,e1,e3);
			}
			printf("sem else\n" );
			break;
		case AssignK:
			//endereço da variavel destino da atribuição
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->child[0]->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->child[0]->attr.name);	

			cGen(t->child[1]);

			//conteudo q sera armazenado na variavel
			e2 = atual;

			e3.tipo = Vazio;
			if(e2.tipo == Const)
				inseriNo(storeIm,e2,e1,e3);	
			else
				inseriNo(store,e2,e1,e3);

				
			break;
		case FuncaoK:
			e2.tipo = Vazio;
			e3.tipo = Vazio;
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->attr.name);	

			inseriNo(funInicio,e1,e2,e3);
			
			cGen(t->child[1]);

			inseriNo(funFim,e1,e2,e3);
			break;
		case VarK:
			//endereco variavel a ser alocada
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->attr.name);

			e2.tipo = Vazio;
			e3.tipo = Vazio;

			inseriNo(allocaMem,e1,e2,e3);
			break;
		default:
			break;
	}
}
/*Nós EXP: VetIdK*/
static void genExp(TreeNode *t)
{
	Endereco e1;
	Endereco e2;
	switch(t->kind.exp)
	{
		case OpK:
			cGen(t->child[0]);
			e1 = atual;
			cGen(t->child[1]);
			e2 = atual;
			char *dest = criaVariavelTemp();
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(dest)*sizeof(char));
			strcpy(atual.conteudo.nome,dest);

			inseriNo(verificaOp(t->attr.op),e1,e2,atual);
			break;
		case VarIdK:
			//endereço da variavel
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->attr.name);

			//endereço do registrador temporario
			char *regTemp = criaVariavelTemp();
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(atual.conteudo.nome,regTemp);

			//endereço vazio
			Endereco e;
			e.tipo = Vazio;

			//operação para carregar varivel da memoria para registrador

			inseriNo(load,e1,atual,e);
			
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
				fprintf(arq, "\n");
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



