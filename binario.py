
import numpy as np

def InstructionFormat(mnemonic):
    pass

def MnemonicExtract(instruction):
    instruction = str(instruction)
    return instruction.split(" ")

def DecimalToBinary(dec,size):
    """binary = bin(dec)
    binary = str(binary).replace("0b","")
    i = 0
    for i in range(0,(size-len(binary))):
        binary = "0"+binary"""

    binary = np.binary_repr(dec, width=size)
    return binary



def ConvertInstruction(instruction):
    instruction = str(instruction).replace("\n","")
    instruction_splited = instruction.split(" ")
    instruction_bin = ""
    rs = ""
    rt = ""
    rd = ""
    immediate = ""
    shamt = ""
    address = ""
    OPCode = "000000"
    funct = ""

    #instruction type R
    if(instruction_splited[0]=='add' 
        or instruction_splited[0]=='sub' or instruction_splited[0]=='sll' or instruction_splited[0]=='srl'
        or instruction_splited[0]=='mult' or instruction_splited[0]=='div' or instruction_splited[0]=='slt'
        or instruction_splited[0]=='and' or instruction_splited[0]=='or' or instruction_splited[0]=='nor'
        or instruction_splited[0]=='mod' or instruction_splited[0]=='jr'):
        
        if(instruction_splited[0]=='add'):
            funct = "000001"
        elif(instruction_splited[0]=='sub'):
            funct = "000010"
        elif(instruction_splited[0]=='mult'):
            funct = "000011"
        elif(instruction_splited[0]=='div'):
            funct = "000100"
        elif(instruction_splited[0]=='and'):
            funct = "000101"
        elif(instruction_splited[0]=='or'):
            funct = "000110"
        elif(instruction_splited[0]=='nor'):
            funct = "000111"
        elif(instruction_splited[0]=='slt'):
            funct = "001000"
        elif(instruction_splited[0]=='sll'):
            funct = "001001"
        elif(instruction_splited[0]=='srl'):
            funct = "001010"
        elif(instruction_splited[0]=='mod'):
            funct = "001011"
        elif(instruction_splited[0]=='jr'):
            funct = "001100"
        
        if(instruction_splited[0]=='jr'):
            rs = DecimalToBinary(int(str(instruction_splited[1]).replace("r","")),5)
            instruction_bin = OPCode+rs+"000000000000000"+funct#concatenate the instruction components
        else:
            rs = DecimalToBinary(int(str(instruction_splited[1]).replace("r","")),5)
            rt = DecimalToBinary(int(str(instruction_splited[2]).replace("r","")),5)
            rd = DecimalToBinary(int(str(instruction_splited[3]).replace("r","")),5)
            if(len(instruction_splited)==5):
                shamt = DecimalToBinary(int(instruction_splited[4]),5)
            else:
                shamt = DecimalToBinary(0,5)

            instruction_bin = OPCode+rs+rt+rd+shamt+funct#concatenate the instruction components
        
    #instruction Type I
    elif(instruction_splited[0]=='addi' or instruction_splited[0]=='multi' or instruction_splited[0]=='divi' or
            instruction_splited[0]=='andi' or instruction_splited[0]=='bltz' or instruction_splited[0]=='bgtz' or
            instruction_splited[0]=='beqz' or instruction_splited[0]=='beq' or instruction_splited[0]=='bne' or
            instruction_splited[0]=='lui' or instruction_splited[0]=='lw' or instruction_splited[0]=='sw' or 
            instruction_splited[0]=='ori' or instruction_splited[0]=='slti' or instruction_splited[0]=='modi'):

        if(instruction_splited[0]=='addi'):
            OPCode = "000001"
        elif(instruction_splited[0]=='multi'):
            OPCode = "000010"
        elif(instruction_splited[0]=='divi'):
            OPCode = "000011"
        elif(instruction_splited[0]=='andi'):
            OPCode = "000100"
        elif(instruction_splited[0]=='bltz'):
            OPCode = "000101"
        elif(instruction_splited[0]=='bgtz'):
            OPCode = "000110"
        elif(instruction_splited[0]=='beqz'):
            OPCode = "000111"
        elif(instruction_splited[0]=='beq'):
            OPCode = "001000"
        elif(instruction_splited[0]=='bne'):
            OPCode = "001001"
        elif(instruction_splited[0]=='lui'):
            OPCode = "010101"
        elif(instruction_splited[0]=='lw'):
            OPCode = "001010"
        elif(instruction_splited[0]=='sw'):
            OPCode = "001011"      
        elif(instruction_splited[0]=='ori'):
            OPCode = "001100"   
        elif(instruction_splited[0]=='slti'):
            OPCode = "001101"   
        elif(instruction_splited[0]=='modi'):
            OPCode = "001110"   
        rs = DecimalToBinary(int(str(instruction_splited[1]).replace("r","")),5)
        rt = DecimalToBinary(int(str(instruction_splited[2]).replace("r","")),5)
        immediate = DecimalToBinary(int(instruction_splited[3]),16)

        instruction_bin = OPCode+rs+rt+immediate
    #instructions type J
    elif(instruction_splited[0]=='jump' or instruction_splited[0]=='jal'):
        if(instruction_splited[0]=='jump'):
            OPCode = "001111"
        elif(instruction_splited[0]=='jal'):
            OPCode = "010000"
        
        address = DecimalToBinary(int(instruction_splited[1]),26)

        instruction_bin = OPCode + address
    
    #instructions type Control
    elif(instruction_splited[0]=='nop' or instruction_splited[0]=='halt'):
        if(instruction_splited[0]=='nop'):
            OPCode = "010001"
        elif(instruction_splited[0]=='halt'):
            OPCode = "010010"
        
        instruction_bin = OPCode + "00000000000000000000000000"
    #output and input instructions
    elif(instruction_splited[0]=='input' or instruction_splited[0]=='output'):
        if(instruction_splited[0]=='input'):
            OPCode = "010011"
            rs = DecimalToBinary(int(str(instruction_splited[1]).replace("r","")),5)
            funct = "010111"
            instruction_bin = OPCode+rs+"000000000000000"+funct
        elif(instruction_splited[0]=='output'):
            OPCode = "010100"
            rs = DecimalToBinary(int(str(instruction_splited[1]).replace("r","")),5)
            instruction_bin = OPCode+rs+"000000000000000000000"

    return instruction_bin  
        
        
        
               
    


def main():
    binario = open("codigoBinario.txt",'w+')
    f = open("assembly.txt", "r")
    for x in f:
        binario.write(ConvertInstruction(x))
        binario.write("\n")
    binario.close()
    print("Geracao do codigo binario concluida...")
if __name__ == '__main__':
    main()
