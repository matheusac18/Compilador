#include "globals.h"
#include "scan.h"
#include "parse.h"
#include "util.h"
#include "analyze.h"
#include "codigoIntermediario.h"



int main(){
	TreeNode *arvoreSintatica;
	printf("Iniciando análise...\n");
	arvoreSintatica = parse();
	if(arvoreSintatica != NULL)
	{
		printTreeR(arvoreSintatica);
		printf("Árvore sintática criada...\n");
		buildSymtab(arvoreSintatica);
		printf("Tabela de simbolos criada...\n");
	}

    gerarIntermediario(arvoreSintatica);
    printf("Codigo intermediario gerado...\n");

}
