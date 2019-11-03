# Microprocessor
> ## Introduction
> The codes of Lecture of Microprocessor are saved in this repository.
After Understanding the operation of Microcontroller CPU,  I made mini CPU(we called it tPU) by C++ codes in Lecture of Microprocessor which is proceeded by Prof. D.J.Park 
Now I'll go on a explaination each code a little bit in detail. 

> ## tPU
> 
> tPU is the abbreviation of Tiny Process Unit. It is composed of 3 Units as  CDecode, CExecute, and CRegsiter. Also, To explain all the processes of tPU, we should add CMemory and CCode. 

> ## Composition
> 1. [CCode](README.md#CCode)
>
> 2. [CDecode](README.md#CDecode)
>
> 3. [CExecute](README.md#CExecute)
>
> 4. [CMemory](README.md#CMemory)
>
> 5. [CRegister](README.md#CRegister)
>
> 
  
  ### **CCode** 

CCode Part is the starting point that can do upload Binary code which has to be executed by tPU. It is implemented by the technique of file i/o in C++ grammar. Let's see the code together. 

```c++
<CCode.h>

#include <iostream>
#pragma once
 
using namespace std;

class CCode{

public: 
	CCode()  {}
	virtual ~CCode() {}

private:
};

class CFlash1KWord : public CCode{
public: 
	CFlash1KWord(char* filename, int line);
	virtual ~CFlash1KWord();
	
	string& code_at(int addr); 
	//string&: reference return type(like pointer)
	
	int code_memory_size(){ return 1024; }

private:
	string m_filename;
	int m_line;
	string m_code[1024];
};

<CCode.cpp>
#include "CCode.h"
#include <iostream>
#include <fstream>

using namespace std;

CFlash1KWord::CFlash1KWord(char* filename, int line){

	m_filename=filename;
	m_line=line;
	ifstream infile;
	infile.open(filename, ios::in);

	int addr = 0;
	char bit;
	while (addr<line){
		for(int i=0; i<16; i++){
			infile >> bit;
			m_code[addr]= m_code[addr] + bit;
			}
		addr++;
	}
	infile.close();
}

CFlash1KWord :: ~CFlash1KWord(){ }

string& CFlash1KWord::code_at(int addr){
	return m_code[addr];
}
```
when the tPU unit gets the .bin file which is written by binary code, you can see the technique of filei/o.  Also, this code has the function which change the string of binary code to real binary number sequence.  

 ### **CDecode**
 ```c++
<CCDecode.h>
 
#include <iostream>
#include "CCode.h" 
#pragma once

using namespace std;
 
enum {R0=0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15};
enum {MOV0=0,MOV1,MOV2,MOV3,ADD,SUB,JZ,MUL,MOV4,MOV5};
class CDecode{
public:
	CDecode(){ }
	virtual ~CDecode(){ }
};

 
typedef struct {
	unsigned int OPCODE : 4;
	unsigned int OP1    : 4;
		 int OP2    : 8;
} SInstruction;

class CT1DecodeDirectFetch : public CDecode{
public:
	CT1DecodeDirectFetch(CFlash1KWord& code) : m_code_memory(code){};
	virtual ~CT1DecodeDirectFetch() { }

	bool do_fetch_from(int PC);
	bool do_decode();
	void show_instruction();

    unsigned int get_opcode() {return m_instruction.OPCODE;}
    unsigned int get_op1() {return m_instruction.OP1;}
             int get_op2() {return m_instruction.OP2;}


private:
 
	CFlash1KWord& m_code_memory;
	string m_inst_buffer;
	SInstruction m_instruction;

};


<CDecode.cpp>

 #include "CDecode.h"

bool CT1DecodeDirectFetch::do_fetch_from(int PC){
	if(PC>=0&& PC < m_code_memory.code_memory_size()){
		m_inst_buffer = m_code_memory.code_at(PC);
        cout << "Fetching from code memory at " << PC << endl;
		return true;
	}
	else 
		return false;
}

void CT1DecodeDirectFetch::show_instruction(){
	if(m_instruction.OPCODE == MOV3){
		cout <<"MOV3 " << "R" << m_instruction.OP1 << ", #" << m_instruction.OP2 << endl;
    }else if(m_instruction.OPCODE == ADD){
		unsigned int op2 = (m_instruction.OP2 >> 4) & 0xF;
        cout <<"ADD " << "R" << m_instruction.OP1 << ", R" << op2 << endl;
    }else if(m_instruction.OPCODE == SUB){
        unsigned int op2 = (m_instruction.OP2 >> 4) & 0xF;
		cout <<"SUB " << "R" << m_instruction.OP1 << ", R" << op2 << endl;
    }else if(m_instruction.OPCODE == MOV0){
        unsigned int op2 = m_instruction.OP2 & 0xFF; //eight bit particle;
		cout <<"MOV0 " << "R" << m_instruction.OP1 << ", [" << op2 << "]" << endl;
    }else if(m_instruction.OPCODE == MOV1){
        unsigned int op2 = m_instruction.OP2 & 0xFF;
		cout <<"MOV1 [" << op2 << "], R" << m_instruction.OP1 << endl;
    }else if(m_instruction.OPCODE == MUL){
        unsigned int op2 =(m_instruction.OP2>>4) & 0xF;
		cout <<"MUL " << "R" << m_instruction.OP1 << " , R" << op2 << endl;
    }else if(m_instruction.OPCODE == JZ){
		cout <<"JZ " << "R" << m_instruction.OP1 << ", " << m_instruction.OP2 << endl;
    }else if(m_instruction.OPCODE == MOV2){
        unsigned int op2 = m_instruction.OP2 & 0xF;
		cout <<"MOV2 [R" << m_instruction.OP1 << "], R" << op2 << endl;
    }else if(m_instruction.OPCODE == MOV4){
        unsigned int op2 = (m_instruction.OP2>>4) & 0xF;
		cout <<"MOV4 R" << m_instruction.OP1 <<", R"<< op2 << endl;
    }else if(m_instruction.OPCODE == MOV5){
        unsigned int op2 = m_instruction.OP2 & 0xF;
        cout <<"MOV5 R" << m_instruction.OP1 <<" [R"<< op2 <<"]"<< endl;
    }
}

bool CT1DecodeDirectFetch::do_decode(){

    int decoded = 0;
        
    // Decoding OPcode
    if(m_inst_buffer[0] == '1')
        decoded |= 8; 
    if(m_inst_buffer[1] == '1')
        decoded |= 4; 
    if(m_inst_buffer[2] == '1')
        decoded |= 2; 
    if(m_inst_buffer[3] == '1')
        decoded |= 1; 

    m_instruction.OPCODE = decoded;

   decoded = 0;
   //Decoding OP1
    if(m_inst_buffer[4] == '1')
        decoded |= 8; 
    if(m_inst_buffer[5] == '1')
        decoded |= 4; 
    if(m_inst_buffer[6] == '1')
        decoded |= 2; 
    if(m_inst_buffer[7] == '1')
        decoded |= 1; 

    m_instruction.OP1 = decoded;

    decoded = 0;
    //Decoding OP2
    if(m_inst_buffer[8] == '1')
        decoded |= 128; 
    if(m_inst_buffer[9] == '1')
        decoded |= 64; 
    if(m_inst_buffer[10] == '1')
        decoded |= 32; 
    if(m_inst_buffer[11] == '1')
        decoded |= 16;
    if(m_inst_buffer[12] == '1')
        decoded |= 8; 
    if(m_inst_buffer[13] == '1')
        decoded |= 4; 
    if(m_inst_buffer[14] == '1')
        decoded |= 2; 
    if(m_inst_buffer[15] == '1')
        decoded |= 1; 
 
    m_instruction.OP2 = decoded;
   
    return true;
}

 ```
 

