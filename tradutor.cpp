#include <iostream> 
#include <fstream>
#include <string>
#include <list> 
#include <iterator> 
#include <vector>
#include <stack>  
#include <sstream>
#include <locale>   

using namespace std; 

enum instrucoes:int {ADD, SUB, MULT, DIV, AND, OR, NOR, SLT, SGT,SLTE, SGET, SET, SDT, SLL, SRL,MOD,JR,ADDI,MULTI,DIVI,ANDI,BLTZ, BGTZ,BEQZ, 
				 BEQ, BNE, LW, SW, ORI, STLI, MODI, JUMP,JAL,NOP,HALT,INPUT,OUTPUT,LUI};
enum registradores:int {$zero, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13, $t14, $t15, $s0, $s1, $s2, $s3, $s4,
					$s5, $s6, $s7, $s8, $s9, $s10, $rf, $fp, $sp,$ra};
string reg_string[] = { "$zero", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "t10", "t11", "t12", "t13", "t14", "t15", "$s0", "$s1", "$s2", "$s3", "$s4",
					       "$s5", "$s6", "$s7", "$s8", "$s9", "$s10", "$rf", "$fp", "$sp", "$ra"};

string inst_string[] = {"add", "sub", "mult", "div", "and", "or", "nor", "slt", "sgt","slte", "sget", "set", "sdt", "sll", "srl","mod","jr","addi","multi","divi","andi","bltz", "bgtz","beqz", 
				 			 "beq", "bne", "lw", "sw", "ori", "slti", "modi", "jump","jal","nop","halt","input","output","lui"};

int numLinha = 0;

class Variavel
{
	public:
	int memLoc;
	string nome;
	int tipo;
};

class Escopo
{
	public:

	int posAtualMem;
	string nome;
	list <Variavel> variaveis;
};

class Label
{
	public:

	int numLinha;
	string nome;
};


struct quadrupla
{
	string op,end1,end2,end3;
};

//lista com as posições das variaveis na memoria
list <Escopo> varLoc;

//lista com o nome e numero da linha da label
list <Label> labels;

//pilha com os parametros
stack<string> parametros;

//arquivo de entrada 
ifstream codInt;

//arquivo de saida
ofstream assemblyFile;



void insereEscopo(string escopo)
{
	cout << "Imprimindo;" << endl;
	Escopo novo;
	novo.nome = escopo;
	novo.posAtualMem = 0;
	varLoc.push_front(novo);
}

void insereLabel(string nome)
{
	Label nova;
	nova.nome = nome;
	nova.numLinha = numLinha;
	labels.push_front(nova);
}

void pegaLinhaIntermediario(quadrupla *quad)
{
	string line;
	if(!codInt.eof())
	{
		getline(codInt,line);
		vector<string> result;
   		stringstream s_stream(line); //create string stream from the string
   		while(s_stream.good()) {
	      	string substr;
	     	getline(s_stream, substr, ','); //get first string delimited by comma
	      	result.push_back(substr);
   		}
	 
	    if(result.size()==4)
	    {
	    	quad->op = result[0];
		    quad->end1 = result[1];
		    quad->end2 = result[2];
		    quad->end3 = result[3];	
	    }
	    else//pega uma linha em branco no fim do arquivo -> evita executar a ultima operação duas vezes
	    {
	    	quad->op = "";
	    }
	}
}

void imprimeFormatoR(instrucoes inst, registradores rs, registradores rt, registradores rd)
{
	string instrucao = inst_string[inst]+' '+reg_string[rs]+' '+reg_string[rt]+' '+reg_string[rd];
	if(assemblyFile.is_open())
	{
		assemblyFile<<instrucao<<endl;
		numLinha++;
	}
}

void imprimeFormatoI(instrucoes inst, registradores rs, registradores rt, string imediato)
{
	cout << "Imprime I" << endl;
	string instrucao = inst_string[inst]+' '+reg_string[rs]+' '+reg_string[rt]+' '+imediato;
	if(assemblyFile.is_open())
	{
		assemblyFile<<instrucao<<endl;
		numLinha++;
	}
}

registradores pegaRegistradorNumero(string r)
{
	for(int i=0;i<32;i++)
	{
		if(r==reg_string[i])
			return (registradores) i;
	}
}

bool ehNumero(string s)
{
	bool result = true;
	for(int i = 0; i<s.size();i++)
	{
		if(!isdigit(s[i]))
		{
			result = false;
			break;
		}
	}

	return result;
}

int main()
{

	codInt.open("intermediario.txt");
	assemblyFile.open("assembly.txt");
	quadrupla quad;
	string line;
	while(!codInt.eof())
	{
		pegaLinhaIntermediario(&quad);
		if(quad.op == "add")
		{
			if(ehNumero(quad.end1) && ehNumero(quad.end2))//os dois operadores são numeros
			{
				imprimeFormatoI(ADDI,$zero,pegaRegistradorNumero(quad.end3),quad.end1);//coloca o primeiro operando no registrador de destino RT
				imprimeFormatoI(ADDI,pegaRegistradorNumero(quad.end3),pegaRegistradorNumero(quad.end3),quad.end2);//soma o segundo operando com o primeiro q está no reg de destino
			}
			else if (!ehNumero(quad.end1) && !ehNumero(quad.end2))//ADD
			{
				imprimeFormatoR(ADD,pegaRegistradorNumero(quad.end1),pegaRegistradorNumero(quad.end2),pegaRegistradorNumero(quad.end3));
			}
			else//ADDI
			{
				if(ehNumero(quad.end1))
				{
					imprimeFormatoI(ADDI,pegaRegistradorNumero(quad.end2),pegaRegistradorNumero(quad.end3),quad.end1);
				}
				else
				{
					imprimeFormatoI(ADDI,pegaRegistradorNumero(quad.end1),pegaRegistradorNumero(quad.end3),quad.end2);
				}
			}
		}
		else if(quad.op == "sub")
		{
			if(ehNumero(quad.end1) && ehNumero(quad.end2))//os dois operadores são numeros
			{
				imprimeFormatoI(ADDI,$zero,pegaRegistradorNumero(quad.end3),quad.end1);//coloca o primeiro operando no registrador de destino RT
				imprimeFormatoI(ADDI,pegaRegistradorNumero(quad.end3),pegaRegistradorNumero(quad.end3),"-"+quad.end2);//adiciona  o sinal de menos ao segundo operando
			}
			else if (!ehNumero(quad.end1) && !ehNumero(quad.end2))//SUB
			{
				imprimeFormatoR(SUB,pegaRegistradorNumero(quad.end1),pegaRegistradorNumero(quad.end2),pegaRegistradorNumero(quad.end3));
			}
			else//subtração com imediato
			{
				if(ehNumero(quad.end1))
				{
					imprimeFormatoI(ADDI,$zero,pegaRegistradorNumero(quad.end3),quad.end1);
					imprimeFormatoR(SUB,pegaRegistradorNumero(quad.end3),pegaRegistradorNumero(quad.end2),pegaRegistradorNumero(quad.end3));
				}
				else
				{
					imprimeFormatoI(ADDI,$zero,pegaRegistradorNumero(quad.end3),quad.end2);
					imprimeFormatoR(SUB,pegaRegistradorNumero(quad.end1),pegaRegistradorNumero(quad.end3),pegaRegistradorNumero(quad.end3));
				}
			}
		}
		else if(quad.op == "mult")
		{

		}
		else if(quad.op == "divisao")
		{

		}
		else if(quad.op == "ifFalso")
		{

		}
		else if(quad.op == "call")
		{

		}
		else if(quad.op == "move")
		{
			imprimeFormatoI(ADDI,pegaRegistradorNumero(quad.end1),pegaRegistradorNumero(quad.end2),"0");
		}
		else if(quad.op == "label_op")
		{
			insereLabel(quad.end1);//insere o label na lista
		}
		else if(quad.op == "storeVet")
		{

		}
		else if(quad.op == "jump")
		{

		}
		else if(quad.op == "slt")
		{

		}
		else if(quad.op == "sgt")
		{

		}
		else if(quad.op == "slet")
		{

		}
		else if(quad.op == "sdt")
		{

		}
		else if(quad.op == "sdt")
		{

		}
		else if(quad.op == "sget")
		{

		}
		else if(quad.op == "funInicio")
		{

		}
		else if(quad.op == "funFim")
		{

		}
		else if(quad.op == "loadVar")
		{

		}
		else if(quad.op == "loadVet")
		{

		}
		else if(quad.op == "storeVar")
		{

		}
		else if(quad.op == "allocaMemVar")
		{
			imprimeFormatoI(ADDI,$sp,$sp,"1");
		}
		else if(quad.op == "allocaMemVet")
		{

		}
		else if(quad.op == "retornaValor")
		{

		}
		else if(quad.op == "param")
		{
			cout << "Empilhando parametro" << endl;
			parametros.push(quad.end1);
		}


	}
	codInt.close();
	assemblyFile.close();
	cout << "Tradução para Assembly concluida..." << endl;
	/*insereEscopo("global");
	Escopo n = varLoc.front();
	cout << n.nome << endl;
	string line;
	codInt.open("intermediario.txt");*/
	

}