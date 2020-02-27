#include "globals.h"
#include "codigoIntermediario.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void cGen(TreeNode *t);

QuadLista inicio = NULL;//lista de quadruplas

const char *OpString[] = {"add","sub","mult","divisao","ifFalso", "call", "move","label_op","storeVet","jump","slt","sgt","slet","sget","set","sdt","funInicio","funFim", "loadVar","loadVet","storeVar","allocaMemVar","allocaMemVet","param","empilha","desempilha"};


Endereco atual;

int temporario = 0;
int label;
int qtdeReg = 16;


int regUso[32];
static void genExp(TreeNode *t);
static void genStmt(TreeNode *t);




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

void verificaReg(char *reg)
{
	char n[3];
	int nReg = -1;
	
	strcpy(n,reg);

	n[0] = '0';

	nReg = atoi(n);
	regUso[nReg] = 0;
	printf("\n%d",nReg);
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


	if(op == add || op == mult || op == sub || op == divisao || op == slt || op == sgt || op == slet || op == sget || op == set || op == sdt || op == storeVet)
	{
		if(e1.tipo == String)
			verificaReg(e1.conteudo.nome);
		if(e2.tipo == String)
				verificaReg(e2.conteudo.nome);
	}
	else if(op == ifFalso || op == loadVet || op == storeVar || op == move)
	{
		if(e1.tipo == String)
			verificaReg(e1.conteudo.nome);
	}


}

void empilhaReg()
{
	int i = 0;
	Endereco e1,e2;
	e2.tipo = Vazio;
	for(i=0;i<qtdeReg;i++)
	{
		if(regUso[i]==1)
		{
			e1.tipo = Const;
			e1.conteudo.val = i;
			inseriNo(empilha,e1,e2,e2);
		}
	}
}

void desempilhaReg()
{
	int i = 0;
	Endereco e1,e2;
	e2.tipo = Vazio;
	for(i=qtdeReg-1;i>=0;i--)
	{
		if(regUso[i]==1)
		{
			e1.tipo = Const;
			e1.conteudo.val = i;
			inseriNo(desempilha,e1,e2,e2);
		}
	}
}

char *criaVariavelTemp()
{
	int n = 0;
	n = temporario%qtdeReg;
	regUso[n] = 1;
	char *temp = (char*) malloc((n%10+2)*sizeof(char));
	char *num = (char*) malloc(((n%10+1)));
	sprintf(num,"%d", n);
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

/*Nós STMT:ReturnK, CallK*/
static void genStmt(TreeNode *t)
{
	Endereco e1;
	Endereco e2;
	Endereco e3;
	TreeNode *k;
	char *lblInicioWhile;
	char *lblFimWhile;
	char *regTemp;
	int qtdeParam;
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
			
			break;
		case AssignK:	

			if(t->child[0]->kind.exp == VarIdK)
			{
				//endereço da variavel destino da atribuição
				e1.tipo = String;
				e1.conteudo.nome = (char*) malloc(strlen(t->child[0]->attr.name)*sizeof(char));
				strcpy(e1.conteudo.nome,t->child[0]->attr.name);	

				cGen(t->child[1]);

				//conteudo q sera armazenado na variavel
				e2 = atual;

				e3.tipo = String;
				e3.conteudo.nome = (char*) malloc(strlen(t->child[0]->escopo)*sizeof(char));
				strcpy(e3.conteudo.nome,t->child[0]->escopo);	
				inseriNo(storeVar,e2,e1,e3);
			}
			else if(t->child[0]->kind.exp == VetIdK)
			{
				//escopo do vetor
				e1.tipo = String;
				e1.conteudo.nome = (char*) malloc(strlen(t->child[0]->escopo)*sizeof(char));
				strcpy(e1.conteudo.nome,t->child[0]->escopo);
				//endereço do vetor
				e2.tipo = String;
				e2.conteudo.nome = (char*) malloc(strlen(t->child[0]->attr.name)*sizeof(char));
				strcpy(e2.conteudo.nome,t->child[0]->attr.name);

				//endereço do registrador temporario
				regTemp = criaVariavelTemp();
				e3.tipo = String;
				e3.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
				strcpy(e3.conteudo.nome,regTemp);

				//operação para carregar o endereço da primeira posição do vetor no registrador
				inseriNo(loadVar,e1,e2,e3);

				//endereço base
				e1 = e3;

				cGen(t->child[0]->child[0]);

				//deslocamento
				e2 = atual;

				//temporario com endereço final
				regTemp = criaVariavelTemp();
				e3.tipo = String;
				e3.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
				strcpy(e3.conteudo.nome,regTemp);

				inseriNo(add,e1,e2,e3);
				cGen(t->child[1]);
				e1 = atual;
				e2 = e3;//registrador que contem o endereço do vetor
				e3.tipo = Vazio;
				inseriNo(storeVet,e1,e2,e3);
			}
			

				
			break;
		case FuncaoK:
			e2.tipo = Vazio;
			e3.tipo = Vazio;
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->attr.name);	

			inseriNo(funInicio,e1,e2,e3);
			cGen(t->child[0]);//parametros
			cGen(t->child[1]);//corpo da função

			inseriNo(funFim,e1,e2,e3);
			break;
		case VarK:
			//escopo variavel a ser alocada
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->escopo)*sizeof(char));
			strcpy(e1.conteudo.nome,t->escopo);

			//endereco variavel a ser alocada
			e2.tipo = String;
			e2.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e2.conteudo.nome,t->attr.name);

			e3.tipo = Vazio;

			inseriNo(allocaMemVar,e1,e2,e3);
			break;
		case VetK:
			//escopo do vetor
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->escopo)*sizeof(char));
			strcpy(e1.conteudo.nome,t->escopo);

			//endereco vetor a ser alocado
			e2.tipo = String;
			e2.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e2.conteudo.nome,t->attr.name);

			e3.tipo = Const;
			if(t->child[0]!=NULL)
			{
				e3.conteudo.val = t->child[0]->attr.val;
			}
			else
			{
				e3.conteudo.val = -1;	
			}
			
			inseriNo(allocaMemVet,e1,e2,e3);
			break;
		case WhileK:
			//insere label de inicio do while
			lblInicioWhile = criaLabel();
			
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(lblInicioWhile)*sizeof(char));
			strcpy(e1.conteudo.nome,lblInicioWhile);
			

			e2.tipo = Vazio;
			e3.tipo = Vazio;
			inseriNo(label_op,e1,e2,e3);

			lblFimWhile = criaLabel();
			//chama cGen para o no de operação filho do while
			cGen(t->child[0]);

			//insere o no ifFalso

			e1 = atual;//endereço que contem o resultado da operação do if

			condFalsa.tipo = String;
			condFalsa.conteudo.nome = (char*) malloc(strlen(lblFimWhile)*sizeof(char));
			strcpy(condFalsa.conteudo.nome,lblFimWhile);	

			e2.tipo = Vazio;

			inseriNo(ifFalso,e1,condFalsa,e2);
			cGen(t->child[1]);//corpo do while
			//salta para o inicio do while
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(lblInicioWhile)*sizeof(char));
			strcpy(e1.conteudo.nome,lblInicioWhile);
			e1.tipo = String;
			
			e2.tipo = Vazio;
			e3.tipo = Vazio;
			inseriNo(jump, e1,e2,e3);
			inseriNo(label_op,condFalsa,e2,e3);
			break;
		case ReturnK:
			if(t->child[0]!=NULL)
			{
				cGen(t->child[0]);
				e1 = atual;

				e2.tipo = String;
				char endRetorno[10] = "$rf";
				e2.conteudo.nome = (char*) malloc(10*sizeof(char));
				strcpy(e2.conteudo.nome,endRetorno);

				e3.tipo = Vazio;

				inseriNo(move,e1,e2,e3);
			}
			break;
		case CallK:
			qtdeParam = 0;
			if(t->child[0]!=NULL)
			{


				k = t->child[0];

				while(k!=NULL)
				{
					qtdeParam = qtdeParam +1;
					
					if(k->nodeKind == StmtK)
					{
						genStmt(k);
					}
					else if(k->nodeKind == ExpK)
					{
						genExp(k);
					}
					e2.tipo = Vazio;
					e3.tipo = Vazio;
					inseriNo(param,atual,e2,e3);
					k = k->sibling;
					
				}
			}

			e1.tipo = String;
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e1.conteudo.nome,t->attr.name);
			e2.tipo = Const;
			e2.conteudo.val = qtdeParam;
			e3.tipo = Vazio;

			char verificaOutput[20];
			char verificaInput[20];
			strcpy(verificaOutput,"output");
			strcpy(verificaInput,"input");

			if(strcmp(verificaOutput,e1.conteudo.nome)!=0 && strcmp(verificaInput,e1.conteudo.nome)!=0)
				empilhaReg();

			inseriNo(call,e1,e2,e3);
			
			if(strcmp(verificaOutput,e1.conteudo.nome)!=0 && strcmp(verificaInput,e1.conteudo.nome)!=0)
				desempilhaReg();
			
			atual.tipo = String;
			char endRetorno[10] = "$rf";
			atual.conteudo.nome = (char*) malloc(10*sizeof(char));
			strcpy(atual.conteudo.nome,endRetorno);
			break;
		default:
			break;
	}
}

static void genExp(TreeNode *t)
{
	Endereco e1;
	Endereco e2;
	Endereco e3;
	char *regTemp;
	switch(t->kind.exp)
	{
		case OpK:
			cGen(t->child[0]);
			e1 = atual;
			cGen(t->child[1]);
			e2 = atual;
			char *dest = criaVariavelTemp();
			e3.tipo = String;
			e3.conteudo.nome = (char*) malloc(strlen(dest)*sizeof(char));
			strcpy(e3.conteudo.nome,dest);

			inseriNo(verificaOp(t->attr.op),e1,e2,e3);
			atual = e3;
			break;
		case VarIdK:
			//escopo da variavel
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->escopo)*sizeof(char));
			strcpy(e1.conteudo.nome,t->escopo);
			//endereço da variavel
			e2.tipo = String;
			e2.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e2.conteudo.nome,t->attr.name);

			//endereço do registrador temporario
			regTemp = criaVariavelTemp();
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
			strcpy(atual.conteudo.nome,regTemp);

			//operação para carregar varivel da memoria para registrador
			inseriNo(loadVar,e1,e2,atual);
			
			break;
		case ConstK:
			atual.tipo = Const;
			atual.conteudo.val = t->attr.val;
			break;
		case VetIdK:

			//escopo do vetor
			e1.tipo = String;
			e1.conteudo.nome = (char*) malloc(strlen(t->escopo)*sizeof(char));
			strcpy(e1.conteudo.nome,t->escopo);
			//endereço do vetor
			e2.tipo = String;
			e2.conteudo.nome = (char*) malloc(strlen(t->attr.name)*sizeof(char));
			strcpy(e2.conteudo.nome,t->attr.name);

			//endereço do registrador temporario
			regTemp = criaVariavelTemp();
			e3.tipo = String;
			e3.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
			strcpy(e3.conteudo.nome,regTemp);

			//operação para carregar o endereço da primeira posição do vetor no registrador
			inseriNo(loadVar,e1,e2,e3);

			//endereço base
			e1 = e3;

			cGen(t->child[0]);

			//deslocamento
			e2 = atual;

			//temporario com endereço final
			regTemp = criaVariavelTemp();
			e3.tipo = String;
			e3.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
			strcpy(e3.conteudo.nome,regTemp);

			inseriNo(add,e1,e2,e3);

			e1 = e3;

			//endereço do registrador temporario
			regTemp = criaVariavelTemp();
			atual.tipo = String;
			atual.conteudo.nome = (char*) malloc(strlen(regTemp)*sizeof(char));
			strcpy(atual.conteudo.nome,regTemp);

			e2.tipo = Vazio;
			inseriNo(loadVet,e1,atual,e2);
			break;
		case TypeK:
			cGen(t->child[0]);
			break;
		default:
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
		fprintf(arq,"%s,",OpString[q->quad.op]);
		switch(q->quad.end1.tipo)
		{
			case Vazio:
				fprintf(arq, "___,");
				break;
			case Const:
				fprintf(arq, "%d,", q->quad.end1.conteudo.val);
				break;
			case String:
				fprintf(arq, "%s,", q->quad.end1.conteudo.nome);
			default:
				break;
		}
		switch(q->quad.end2.tipo)
		{
			case Vazio:
				fprintf(arq, "___,");
				break;
			case Const:
				fprintf(arq, "%d,", q->quad.end2.conteudo.val);
				break;
			case String:
				fprintf(arq, "%s,", q->quad.end2.conteudo.nome);
			default:
				break;
		}
		switch(q->quad.end3.tipo)
		{
			case Vazio:
				fprintf(arq, "___\n");
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
	}
	while(q != NULL);
}

void gerarIntermediario(TreeNode *t)
{
	int i = 0;
	for(i=0;i<qtdeReg;i++)
	{
		regUso[i] = 0;
	}
	cGen(t);
	imprimeIntemediario();
}



