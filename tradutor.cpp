#include <iostream> 
#include <list> 
#include <iterator> 

using namespace std; 

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

//listca com as posições das variaveis na memoria
list <Escopo> varLoc;

void insereEscopo(string escopo)
{
	cout << "Imprimindo;" << endl;
	Escopo novo;
	novo.nome = escopo;
	novo.posAtualMem = 0;
	varLoc.push_front(novo);
}

int main()
{
	insereEscopo("global");
	Escopo n = varLoc.front();
	cout << n.nome << endl;
}