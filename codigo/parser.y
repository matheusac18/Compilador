%{
#define YYPARSER
#define YYDEBUG 1 
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static int yylex();
void yyerror(char *msg);

TreeNode *parse(void);

#define YYSTYPE TreeNode *
static TreeNode *savedTree; /*armazena a árvore sintática para retorno*/

%}

%start programa
%token VOID INT IF ELSE WHILE RETURN
%left SOMA SUB
%left MUL DIV
%token APR FPR
%token NUM ESP ID ACOL FCOL ACH FCH ATRIB IGL DIF MAIGL MEIGL MAI MEN VIRG PV NL ERRO FIM



%%

programa :	declaracao_lista
            {
                savedTree = $1;
            };

declaracao_lista:   declaracao_lista declaracao
                    {
                        YYSTYPE t = $1;
                        if(t != NULL)
                        {
                            while(t->sibling != NULL) t = t->sibling;
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else
                            $$ = $2;
                    }
    |               declaracao
                    {
                        $$ = $1;
                    };

declaracao:         var_declaracao
                    {
                        $$ = $1;
                    }
    |               fun_declaracao
                    {
                        $$ = $1;
                    };

var_declaracao:     tipo_especificador id PV
                    {
						$$ = $1;
                        $$->child[0] = newStmtNode(VarK);
						$$->child[0]->type = $$->type;
						$$->child[0]->attr.name = copyString($2->attr.name);
                        $$->child[0]->lineno = $2->lineno;
                        $$->child[0]->escopo = copyString("global");
                    }
    |              	tipo_especificador  id ACOL vet_num FCOL PV
                    {
						$$ = $1;
                        $$->child[0] = newStmtNode(VetK);
						$$->child[0]->child[0] = $4;
						$$->child[0]->type = $$->type;
						$$->child[0]->attr.name = copyString($2->attr.name);
                        $$->child[0]->lineno = $2->lineno;
                        $$->child[0]->escopo = copyString("global");
                    };

vet_num: NUM
        {
            $$ = newExpNode(ConstK);
            $$->attr.val = atoi(tokenString);
            $$->type = Integer;
        };

fun_declaracao:     tipo_especificador id APR params FPR composto_decl
                    {
                        $$ = $1;
                        $$->child[0] = newStmtNode(FuncaoK);
                        $$->child[0]->child[0] = $4;
						$$->child[0]->child[1] = $6;
						$$->child[0]->type = $$->type;
						$$->child[0]->attr.name = copyString($2->attr.name);
                        $$->child[0]->lineno = $2->lineno;
                        $$->child[0]->escopo = copyString("global");
                        insereEscopo($$->child[0]->child[0],$2->attr.name);
                        insereEscopo($$->child[0]->child[1] = $6,$2->attr.name);

                    };


tipo_especificador: INT
                    {
                        $$ = newExpNode(TypeK);
                        $$->type = Integer;
                    }
    |               VOID
                    {   
                        $$ = newExpNode(TypeK);
                        $$->type = Void;
                    };

params:             param_lista
                    {
                        $$ = $1;
                    }
    |               VOID
                    {
                        $$ = NULL;
                    };

param_lista:        param_lista VIRG param
                    {
                        YYSTYPE t = $1;
                        if(t != NULL)
                        {
                            while(t->sibling != NULL)t = t->sibling;
                            t->sibling = $3;
                            $$ = $1;
                        }
                        else
                            $$ = $3;
                    }
    |               param
                    {
                        $$ = $1;
                    };

param:              tipo_especificador id
                    {
						$$ = $1;
                        $$->child[0] = newStmtNode(VarK);
						$$->child[0]->type = $$->type;
						$$->child[0]->attr.name = copyString($2->attr.name);
                        $$->child[0]->lineno = $2->lineno;
                    }
    |               tipo_especificador id ACOL FCOL
                    {
						$$ = $1;
                        $$->child[0] = newStmtNode(VetK);
						$$->child[0]->type = $$->type;
						$$->child[0]->attr.name = copyString($2->attr.name);
                        $$->child[0]->lineno = $2->lineno;
	                };


composto_decl:      ACH local_declaracao statement_lista FCH
                    {
                        YYSTYPE t = $2;
                        if(t != NULL)
                        {
                            while(t->sibling != NULL)t = t->sibling;
                            t->sibling = $3;
                            $$ = $2;
                        }
                        else
                            $$ = $3;
                    }
    |               ACH local_declaracao FCH
                    {
                        $$ = $2;
                    }
    |               ACH statement_lista FCH {
                        $$ = $2;
                    } 
    |                ACH FCH {
						$$ = NULL;
                    };

local_declaracao:   local_declaracao var_declaracao
                    {
                        YYSTYPE t = $1;

                        if(t!=NULL)//já existem outras declarações
                        {

                            while(t->sibling!=NULL) t = t->sibling;
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else//primeira declaração
                        {
                            $$ = $2;
                        }
                    }
    				|var_declaracao 
    				{
    					$$ = $1;
                    }; 

statement_lista:    statement_lista statement
                    {
                        YYSTYPE t = $1;
                        if(t!=NULL)//ja tem um statement na lista
                        {
                            while(t->sibling!=NULL) t = t->sibling;//percorre os irmaos até achar um t q não tenha irmao
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else{//primeiro statement
                            $$ = $2;
                        }

                    }
    				|statement
    				{
    					$$ = $1;
                    };


statement:          expressao_decl
                    {
                        $$ = $1;
                    }
    |               composto_decl
                    {
                        $$ = $1;
                    }
    |               selecao_decl
                    {
                        $$ = $1;
                    }
    |               iteracao_decl
                    {
                        $$ = $1;
                    }
    |               retorno_decl
                    {
                        $$ = $1;
                    };

expressao_decl:     expressao PV
                    {
                        $$ = $1;
                    }
    |               PV
                    {
						$$ = NULL;
                    };


selecao_decl:       IF APR expressao FPR statement
                    {
                        $$ = newStmtNode(IfK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                    }
    |               IF APR expressao FPR statement ELSE statement
                    {
                        $$ = newStmtNode(IfK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                        $$->child[2] = $7;
                    };

iteracao_decl:      WHILE APR expressao FPR statement
                    {
                        $$ = newStmtNode(WhileK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                    };

retorno_decl:       RETURN PV
                    {
                        $$ = newStmtNode(ReturnK);
                    }
    |               RETURN expressao PV
                    {
                        $$ = newStmtNode(ReturnK);
                        $$->child[0] = $2;
                    };

expressao:          var ATRIB expressao
                    {
                        
                        $$ = newStmtNode(AssignK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
    |               simples_expressao
                    {
                        $$ = $1;
                    };

var:                id
					{
						$$ = $1;
						$$->kind.exp = VarIdK;
						$$->type = Integer;
					}
    |               id ACOL expressao FCOL
                    {
                        $$ = $1;
                        $$->child[0] = $3;
                        $$->kind.exp = VetIdK;
                        $$->type = Integer;
                    };


simples_expressao:  soma_expressao relacional soma_expressao
                    {
                        $$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
    |               soma_expressao
                    {
                        $$ = $1;
                    };

relacional:         MEIGL
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = MEIGL;
                    }
    |               MEN
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = MEN;
                    }
    |               MAI
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = MAI;
                    }
    |               MAIGL
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = MAIGL;
                    }
    |               IGL
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = IGL;
                    }
    |               DIF
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = DIF;
                    };

soma_expressao:     soma_expressao soma termo
                    {
                        $$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
    |               termo
                    {   
                        $$ = $1;
                    };

soma:               SOMA
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = SOMA;
                        $$->type = Integer;
                    }
    |               SUB
                    {
                        $$ = newExpNode(OpK);
                        $$->attr.op = SUB;
                        $$->type = Integer;
                    };

termo:              termo mult fator
                    {
                        $$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
    |               fator
                    {
                        $$ = $1;
                    };

mult:               MUL
                    {
                        $$ = newExpNode(OpK);
                        $$->type = Integer;
                        $$->attr.op = MUL;
                    }
    |               DIV
                    {
                        $$ = newExpNode(OpK);
                        $$->type = Integer;
                        $$->attr.op = DIV;
                    };

fator:              APR expressao FPR {$$ = $2;}
    |               var{$$ = $1;}
    |               ativacao{$$ = $1;}
    |               NUM
                    {
                        $$ = newExpNode(ConstK);
            			$$->attr.val = atoi(tokenString);
            			$$->type = Integer;
                    };

ativacao:           id APR args FPR
                    {   
                        $$ = newStmtNode(CallK);
                        $$->child[0] = $3;
                        $$->attr.name = copyString($1->attr.name);
                        $$->lineno = $1->lineno;
                    }
                    
    |               id APR FPR               
                    {
                        $$ = newStmtNode(CallK);
                        $$->attr.name = copyString($1->attr.name);
                        $$->lineno = $1->lineno;
                    };

args:               arg_lista {$$ = $1;};

arg_lista:          arg_lista VIRG expressao
                    {
                        YYSTYPE t = $1;
                        if(t!=NULL){
                            while(t->sibling != NULL) t = t->sibling;
                            t->sibling = $3;
                            $$ = $1;
                        }
                        else
                        {
                            $$ = $3;
                        }

                    }
    |               expressao
                    {
                        $$ = $1;
                    };


id: ID
    {
        $$ = newExpNode(IdK);
        $$->attr.name = copyString(tokenString);
        $$->lineno = lineno;
    };


%%

void yyerror(char *msg)
{
	if(yychar == ERRO)
	{
		exit(0);
	}
	else
	{
		char *t = printToken(yychar,tokenString);
	   	printf("ERRO SINTÁTICO: %s Linha %d\n",t,lineno);
	}
}

static int yylex(void)
{
	TokenType t = getToken();
	return t;
}

TreeNode *parse(void)
{
	yyparse();
    return savedTree;
}


/*int main(void)
{
	printf("Parser em execução...\n");
	TreeNode *t = parse();
	printTreeR(t);
	printf("Fim da analise.\n");
}*/
