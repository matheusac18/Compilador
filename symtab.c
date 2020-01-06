#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "util.h"


//tamanho da tabela hash
#define SIZE 211

#define SHIFT 4

/* função de hash */
static int hash (char *key)
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0'){
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

//lista encadeada para armazenar as linhas
typedef struct LineListRec{
    int lineno;
    struct LineListRec *next;
}*LineList;

//membro da tabela hash
typedef struct HashCel{
    char *nome;
    LineList lines;
    char *escopo;
    int tipoDados; // 0 void 1 int
    int ehVar; //0 função 1 variável
    struct HashCel * next;
}*HashList;

/*tabela hash*/
static HashList hashTable[SIZE];


void st_insert(char *nome, char *escopo, int tipoDados, int ehvar, int lineno)
{
    char nomeConcatenado[100];
    strcpy(nomeConcatenado,"");
    strcat(nomeConcatenado,nome);
    strcat(nomeConcatenado,escopo);

    int h = hash(nomeConcatenado);
    
    HashList l =  hashTable[h];
    while ((l != NULL) && (strcmp(nome,l->nome) != 0))
        l = l->next;

    if (l == NULL){ /* variavel/funcao ainda não foi inserida na tabela */
        l = (HashList) malloc(sizeof(struct HashCel));
        l->nome = nome;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->ehVar = ehvar;
        l->tipoDados = tipoDados;
        l->escopo = copyString(escopo);
        l->lines->next = NULL;
        l->next = hashTable[h];
        hashTable[h] = l;
    } else {
     /* variavel/funcao já presente na tabela, insere apenas a linha */
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

//verifica se a variavel/funcao está presente na tabela hash
int st_lookup(char *nome, char *escopo)
{
    char nomeConcatenado[100];
    strcpy(nomeConcatenado,"");
    strcat(nomeConcatenado,nome);
    strcat(nomeConcatenado,escopo);
    int h = hash(nomeConcatenado);
    HashList l =  hashTable[h];
    while ((l != NULL) && (strcmp(nome,l->nome) != 0))
        l = l->next;
    if (l == NULL) return 0;
    else return 1;
}

int pegaTipoFuncao(char *nome, char *escopo)
{
    char nomeConcatenado[100];
    strcpy(nomeConcatenado,"");
    strcat(nomeConcatenado,nome);
    strcat(nomeConcatenado,escopo);
    int h = hash(nomeConcatenado);
    HashList l =  hashTable[h];
    while ((l != NULL) && (strcmp(nome,l->nome) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->tipoDados;
}

//funcao para imprimir a tabela hash
void printSymTab(FILE *listing)
{
    int i;
    int size = 0, j = 0;
    fprintf(listing,"                    Nome                   Var/func  Tipo         Escopo                       Número de linhas\n");
    fprintf(listing,"-----------------------------------------  --------  ----  --------------------  ------------------------------------------------\n");
    for (i=0;i<SIZE;++i){
        if (hashTable[i] != NULL){
        HashList l = hashTable[i];
            while (l != NULL){
                LineList t = l->lines;
                fprintf(listing,"%s",l->nome);
                size = strlen(l->nome);
                for(j=0;j<43-size;j++)
                    fprintf(listing," ");

                if(l->ehVar)
                    fprintf(listing,"variável  ");
                else 
                    fprintf(listing,"funcao    ");

                if(l->tipoDados) 
                    fprintf(listing,"int   ");
                else 
                    fprintf(listing,"void  ");

                fprintf(listing,"%s ",l->escopo);
                size = strlen(l->escopo);
                for(j=0;j<21-size;j++)
                    fprintf(listing," ");

				while (t != NULL){
                    fprintf(listing,"%d, ",t->lineno);
                    t = t->next;
                }
                fprintf(listing,"\n");
                l = l->next;
            }
        }
    }
}
