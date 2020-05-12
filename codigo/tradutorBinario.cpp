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

struct instrucao
{
	string op;
    string campo1;
    string campo2;
    string campo3;
};



//arquivo de entrada
ifstream assembly;

//arquivo de saida
ofstream binario;

string DecimalBinario(string decimal, int tamanho)
{
    char binario[tamanho];
    int d=0, i=tamanho-1, n=0;
    n = stoi(decimal);
    d = abs(n);
    for(i=0;i<tamanho;i++)
    {
      binario[i] = '0';
    }

    i=tamanho-1;

    while(d>=2)
    { 
        binario[i] = '0'+(d%2);
        d = d/2;
        i--;      
    }

    binario[i] = '0'+(d);


    if(n<0)//numero negativo
    {
        //inverte binario
        for(i=0;i<tamanho;i++)
        {
          if(binario[i] == '0')
            binario[i] = '1';
          else
            binario[i] = '0';
        }

        if(binario[tamanho-1]=='0')
          binario[tamanho-1] = '1';
        else{
          i = tamanho-1;
          while(binario[i] == '1')
          {
            binario[i] = '0';
            i--;
          }
          binario[i] = '1';
        }
    }

    //converte para string
    string resultado;
    for (i = 0; i < tamanho; i++) { 
          resultado = resultado + binario[i]; 
    } 
    return resultado;

}


int pegaInstrucaoAssembly(instrucao *inst)
{
    string linha;
	if(!assembly.eof())
	{
		getline(assembly,linha);
		vector<string> result;
   		stringstream s_stream(linha);
   		while(s_stream.good()) 
        {
	      	string substr;
	     	getline(s_stream, substr, ' '); 
	      	result.push_back(substr);
   		}

	    if(result.size()>1)
        {
            inst->op = result[0];
            inst->campo1 = result[1];
            if(result.size()>=3)
                inst->campo2 = result[2];
            if(result.size()>=4)
                inst->campo3 = result[3];
            return 1;
        }
        else
        {
            return -1;
        }
        
	}
}

string converteInstrucao(instrucao *inst)
{
    string rs, rt, rd, imediato, end, opcode, funct, instrucaoBinario;

    //instrucoes do tipo R
    if(inst->op=="add" || inst->op=="sub" || inst->op=="sll" 
        || inst->op=="srl"|| inst->op=="mult" || inst->op=="div" 
        || inst->op=="slt"|| inst->op=="and" || inst->op=="or"
        || inst->op=="nor"|| inst->op=="mod" || inst->op=="jr")
    {
        opcode = "000000";
        if(inst->op=="add")
            funct = "000001";
        else if(inst->op=="sub")
            funct = "000010";
        else if(inst->op=="mult")
            funct = "000011";
        else if(inst->op=="div")
            funct = "000100";
        else if(inst->op=="and")
            funct = "000101";
        else if(inst->op=="or")
            funct = "000110";
        else if(inst->op=="nor")
            funct = "000111";
        else if(inst->op=="slt")
            funct = "001000";
        else if(inst->op=="sll")
            funct = "001001";
        else if(inst->op=="srl")
            funct = "001010";
        else if(inst->op=="mod")
            funct = "001011";
        else if(inst->op=="jr")
            funct = "001100";
        else if(inst->op=="sgt")
            funct = "001101";
        else if(inst->op=="slet")
            funct = "001110";
        else if(inst->op=="sget")
            funct = "001111";
        else if(inst->op=="set")
            funct = "010000";
        else if(inst->op=="sdt")
            funct = "010001";

        if(inst->op == "jr")
        {
            rs = DecimalBinario(inst->campo1.replace(0,1,""),5);
            instrucaoBinario = opcode+rs+"000000000000000"+funct;
        }
        else
        {
            rs = DecimalBinario(inst->campo1.replace(0,1,""),5);
            rt = DecimalBinario(inst->campo2.replace(0,1,""),5);
            rd = DecimalBinario(inst->campo3.replace(0,1,""),5);
            instrucaoBinario = opcode+rs+rt+rd+"00000"+funct;
        }
    }
    //instrucoes do tipo I
    else if(inst->op=="addi" || inst->op=="multi" || inst->op=="divi" ||
            inst->op=="andi" || inst->op=="bltz"  || inst->op=="bgtz" ||
            inst->op=="beqz" || inst->op=="beq"   || inst->op=="bne"  ||
            inst->op=="lui"  || inst->op=="lw"    || inst->op=="sw"   || 
            inst->op=="ori"  || inst->op=="slti"  || inst->op=="modi" ||
            inst->op=="sgti" || inst->op=="sleti" || inst->op=="sgeti"||
            inst->op=="seti" || inst->op=="sdti")
    {
        if(inst->op=="addi")
            opcode = "000001";
        else if(inst->op=="multi")
            opcode = "000010";
        else if(inst->op=="divi")
            opcode = "000011";
        else if(inst->op=="andi")
            opcode = "000100";
        else if(inst->op=="bltz")
            opcode = "000101";
        else if(inst->op=="bgtz")
            opcode = "000110";
        else if(inst->op =="beqz")
            opcode = "000111";
        else if(inst->op=="beq")
            opcode = "001000";
        else if(inst->op=="bne")
            opcode = "001001";
        else if(inst->op=="lui")
            opcode = "010101";
        else if(inst->op=="lw")
            opcode = "001010";
        else if(inst->op=="sw")
            opcode = "001011";
        else if(inst->op=="ori")
            opcode = "001100";
        else if(inst->op=="slti")
            opcode = "001101";
        else if(inst->op=="modi")
            opcode = "001110";
        else if(inst->op=="sgti")
            opcode = "010110";
        else if(inst->op=="sleti")
            opcode = "010111";
        else if(inst->op=="sgeti")
            opcode = "011000";
        else if(inst->op=="seti")
            opcode = "011001";
        else if(inst->op=="sdti")
            opcode = "011010";
        rs = DecimalBinario(inst->campo1.replace(0,1,""),5);
        rt = DecimalBinario(inst->campo2.replace(0,1,""),5);
        imediato = DecimalBinario(inst->campo3,16);
        instrucaoBinario = opcode+rs+rt+imediato;
    }
    //instrucoes do tipo J
    else if(inst->op=="jump")
    {
        opcode = "001111";
        end = DecimalBinario(inst->campo1,26);
        instrucaoBinario = opcode+end;
    }
    else if(inst->op=="jal"){
        opcode = "010000";
        end = DecimalBinario(inst->campo1,26);
        instrucaoBinario = opcode+end;
    }
    //instrucoes de entrada/saida
    else if(inst->op=="input")
    {
        opcode = "010011";
        rs = DecimalBinario(inst->campo1.replace(0,1,""),5);
        funct = "010111";
        instrucaoBinario = opcode+rs+"000000000000000"+funct;
    }
    else if(inst->op=="output")
    {
        opcode = "010100";
        rs = DecimalBinario(inst->campo1.replace(0,1,""),5);
        instrucaoBinario = opcode+rs+"000000000000000000000";
    }

    return instrucaoBinario;
}

int main()
{
    assembly.open("assembly.txt");
    binario.open("codigoBinario.txt");
    instrucao atual;
    string instrucaoBinario = "";
    int resultadoAssembly = 0;
    while(!assembly.eof())
    {
        resultadoAssembly = pegaInstrucaoAssembly(&atual);
        if(resultadoAssembly!=-1)
        {
            instrucaoBinario = converteInstrucao(&atual);
            binario << instrucaoBinario << endl;
        }
    }
    cout<<"Codigo binario finalizado!"<<endl;
    return 0;
}