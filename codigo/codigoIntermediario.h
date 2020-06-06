#ifndef _CODIGOINTERMEDIARIO_H_
#define _CODIGOINTERMEDIARIO_H_
#include "globals.h"

typedef enum {add,sub,mult,divisao,ifFalso, call, move,label_op, 
storeVet,jump,slt,sgt,slet,sget,set,sdt, 
funInicio, funFim, loadVar,loadVet, storeVar, allocaMemVar, allocaMemVet,
param,empilha,desempilha}Operacao;

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


void gerarIntermediario(TreeNode *t);

#endif
