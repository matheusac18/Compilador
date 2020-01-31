#ifndef _CODIGOINTERMEDIARIO_H_
#define _CODIGOINTERMEDIARIO_H_
#include "globals.h"

typedef enum {add,sub,mult,divisao,maiorQue, menorQue, ifFalso, call, move,label_op, storeIm,jump,slt,sgt,slet,sget,set,sdt, funInicio, funFim, loadVar,loadVet, store, allocaMemVar, allocaMemVet,retornarValor}Operacao;

typedef enum {Vazio,Const,String} TipoEnd;

typedef struct{
	TipoEnd tipo;
	union{
		int val;
		char *nome;
	}conteudo;
	int tamanho;
}Endereco;

typedef struct 
{
	Operacao op;
	Endereco end1, end2, end3;
}Quad;

typedef struct QuadListaNo{
  int loc;
  Quad quad;
  struct QuadListaNo *prox;
} * QuadLista;

void gerarIntermediario(TreeNode *t);
char *criaVariavelTemp();

#endif
