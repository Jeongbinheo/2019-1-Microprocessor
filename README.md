# Microprocessor
> ## Introduction
2019SS ELEC323 마이크로프로세서 수업에서 배운 Mini CPU(이하 tPU)를 C++ 언어를 이용해 구현한 코드들에 대한 설명입니다.
CPU, RAM, MEMORY의 기능과 동작을 이해하고, 교수님께서 수업을 진행하면서 가르쳐주신 코드임을 먼저 밝혀드립니다.

> ## tPU
 
tPU는 Tiny Process Unit의 약자로, 크게 CDecode, CExecute, and CRegsiter 3부분으로 분류 될 수 있습니다. 

또한, tPU의 동작을 완전하게 설명하기 위해서는 CMemory와 CCode라는 부분도 추가적으로 필요합니다.

>## Structure of tPU by Image
> ![structure](https://blogfiles.pstatic.net/MjAxOTExMDNfMzEg/MDAxNTcyNzYyNjkwOTA3.wLwguXI5r4-XB7BHpWT0FR598g-_hOhOz96T2oEXdRQg.BLTL2pKgojD6CwGj4BhjYXIunjdaFCKgG-0-a_qR3r8g.JPEG.hdh988/tPU.jpg?type=w2)


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
 
 
 
  > **CCode** 
   CCode 는 tPU가 실행할 Binary code를 업로드하는 과정을 구현한 부분으로

   C++ 언어의 file i/o 문법을 사용하여 구현하였습니다.

   헤더파일에는 두가지 Class의 함수, 변수에 대해 기술되어 있고,

   .cpp파일에는 메모장파일(.bin)으로 부터 읽어온 Character를 숫자열로 변환하는 기능을 구현함.

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
when the tPU unit gets the .bin file which is written by binary code, you can see the technique of filei/o.
Also, this code has the function which change the string of binary code to real binary number sequence.  


   > **CDecode**
 ```c++
** <CCDecode.h> **
 
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
  
 ### **CExecute**
 ```c++
 
 <CExecute.h>
 
#include <iostream>
#include <stdlib.h>
#include "CDecode.h"
#include "CRegister.h"
#include "CMemory.h"
#pragma once

using namespace std;

class CExecute {
public:
    CExecute(){}
    virtual ~CExecute(){}
};

class CT1ExecuteTinyUnit: public CExecute{
public:
    CT1ExecuteTinyUnit(CT1DecodeDirectFetch& decode,C16RegisterFile& regs,CSRAM_256w& mems) 
    : m_decode_unit(decode), m_regs(regs), m_mems(mems){ }//initiate private member value
    // m_deocde_unit = decode;
    // m_regs = regs;
    virtual ~CT1ExecuteTinyUnit(){ }

    int do_execute();

private:

    CT1DecodeDirectFetch& m_decode_unit;
    C16RegisterFile&      m_regs;
    CSRAM_256w&           m_mems;

};

<CExecute.cpp>

#include "CExecute.h"

int clks[10] = {8,8,12,6,4,4,12,30,2,12};

int CT1ExecuteTinyUnit::do_execute(){
   //MOV3 R1 #2 
    if(m_decode_unit.get_opcode() == MOV3){

       unsigned int reg_index = m_decode_unit.get_op1();
                int data = m_decode_unit.get_op2();
       m_regs.write_on_reg(reg_index,data);
       
       m_regs.set_PC(m_regs.get_PC()+1);   
        return clks[MOV3];

        }else if(m_decode_unit.get_opcode() == ADD){

            unsigned int reg_n = m_decode_unit.get_op1();
            unsigned int reg_m = (m_decode_unit.get_op2() >> 4) & 0XF; //4bit masking course
     
            int Rn = m_regs.read_from_reg(reg_n);
            int Rm = m_regs.read_from_reg(reg_m);

            Rn = Rn + Rm;
       
            m_regs.write_on_reg(reg_n, Rn);
    
            m_regs.set_PC(m_regs.get_PC()+1);   
             
            return clks[ADD];


             }else if(m_decode_unit.get_opcode() == SUB){

                unsigned int reg_n = m_decode_unit.get_op1();
                unsigned int reg_m = (m_decode_unit.get_op2() >> 4)& 0XF;
     
                int Rn = m_regs.read_from_reg(reg_n);
                int Rm = m_regs.read_from_reg(reg_m);

                Rn = Rn - Rm;
       
                m_regs.write_on_reg(reg_n, Rn);
                 
                m_regs.set_PC(m_regs.get_PC()+1);   
               
                return clks[SUB];
               
    
                }else if(m_decode_unit.get_opcode() == MOV0){

                    unsigned int reg_n = m_decode_unit.get_op1();
                    unsigned int mem_addr = m_decode_unit.get_op2()& 0xFF; 
                    //8bit masking course 
     
                    int memory_data = m_mems.read_from_memory(mem_addr);
                    
                    m_regs.write_on_reg(reg_n, memory_data);
                       
                    m_regs.set_PC(m_regs.get_PC()+1);   
                    return clks[MOV0];
        
                    

                }else if(m_decode_unit.get_opcode() == MOV1){

                    unsigned int reg_n = m_decode_unit.get_op1();
                    unsigned int mem_addr = m_decode_unit.get_op2()& 0xFF;
     
                    int Reg_data = m_regs.read_from_reg(reg_n);
                    
                    m_mems.write_on_memory(mem_addr,Reg_data);
                       
                    m_regs.set_PC(m_regs.get_PC()+1);   
                    return clks[MOV1];
    
   

                }else if(m_decode_unit.get_opcode() == MUL){
                     unsigned int reg_n = m_decode_unit.get_op1();
                     unsigned int reg_m = (m_decode_unit.get_op2()>>4)& 0xf;

                     int Rn = m_regs.read_from_reg(reg_n);
                     int Rm = m_regs.read_from_reg(reg_m);

                     Rn = Rn * Rm;

                     m_regs.write_on_reg(reg_n, Rn);

                     m_regs.set_PC(m_regs.get_PC()+1);

                    return clks[MUL];
                                                 
                }else if(m_decode_unit.get_opcode() == JZ){
                     unsigned int reg_n = m_decode_unit.get_op1();
                              int offset = m_decode_unit.get_op2();

                     int Rn = m_regs.read_from_reg(reg_n);
                     m_regs.set_PC(m_regs.get_PC()+1);

                     if(Rn == 0){
                        int pc = m_regs.get_PC();
                        m_regs.set_PC(pc+offset);
                     }

                     return clks[JZ];
                   
                }else if(m_decode_unit.get_opcode() == MOV2){

                    unsigned int reg_n = m_decode_unit.get_op1();
                    unsigned int reg_m = m_decode_unit.get_op2()& 0xF;
                    int reg_n_data = m_regs.read_from_reg(reg_n); 
                    int reg_m_data = m_regs.read_from_reg(reg_m);
                    if(reg_n_data > 255){
                        cout <<"Cannot execute MOV2" << endl;
                        exit(-1);
                    }
                    else m_mems.write_on_memory(reg_n_data,reg_m_data);

                    m_regs.set_PC(m_regs.get_PC()+1);   
                    
                    return clks[MOV2];
                    
                }else if(m_decode_unit.get_opcode() == MOV4){

                    unsigned int reg_n = m_decode_unit.get_op1();
                    unsigned int reg_m = (m_decode_unit.get_op2()>>4)& 0xF;
     
                    int reg_m_data = m_regs.read_from_reg(reg_m);
                    m_regs.write_on_reg(reg_n,reg_m_data);

                    m_regs.set_PC(m_regs.get_PC()+1);   
                    return clks[MOV4];
    
                
                }else if(m_decode_unit.get_opcode() == MOV5){

                    unsigned int reg_n = m_decode_unit.get_op1();
                    unsigned int reg_m = m_decode_unit.get_op2()& 0xF;
                    int reg_m_data = m_regs.read_from_reg(reg_m);
                    if(reg_m_data > 255){
                        cout << "Cannot execute MOV5" <<endl;
                        exit(-1);
                    }else{
                       int data_in_memory_regm_data = m_mems.read_from_memory(reg_m_data);
                    
                       m_regs.write_on_reg(reg_n,data_in_memory_regm_data);
                    }   
                    m_regs.set_PC(m_regs.get_PC()+1); 
                       
                    return clks[MOV5];
                
               }else{
                       cout<<"not executable instruction, not yet implemented."<<endl;
                                 return false;
               }
    
}
```

### **CMemory**
```c++
<CMemory>

#include <iostream>

#pragma once

using namespace std;

class CMemory{
public: 
    CMemory(){}
    virtual ~CMemory(){}
};

class CSRAM_256w: public CMemory{

public: 
    CSRAM_256w(){}
    virtual ~CSRAM_256w(){}

    void write_on_memory(unsigned int index, int data){ m_mems[index] = data; }; 
    int read_from_memory(unsigned int index) {return m_mems[index];};
    
    void show_mems(unsigned int start_addr, unsigned int end_addr);

private:
    int m_mems[256];
  
};

<CMemory.cpp>

#include "CMemory.h"

void CSRAM_256w :: show_mems(unsigned int start_addr, unsigned int end_addr){

    
    cout<< "------ memory Dump (addr: " << start_addr << "~" << end_addr << " )" << endl;
    
    for(unsigned int i = start_addr; i<=end_addr; i++){
        cout << read_from_memory(i) << " ";
    }
    cout << endl; 
};

```
### **CRegister**
```c++

<CRegsiter.h>

#include <iostream>
#pragma once
using namespace std;

class CRegister {
public:
    CRegister(){ }
    virtual ~CRegister(){ }
};

class C16RegisterFile : public CRegister{
public:
    C16RegisterFile():m_PC(0) {}
    virtual ~C16RegisterFile() {}

    void write_on_reg(unsigned int index, int data){m_regs[index] = data;}
    int read_from_reg(unsigned int index)          {return m_regs[index];}
    
    int get_PC(){return m_PC;}
    void set_PC(int pc){m_PC = pc;}
    void show_regs();

private:
    int m_regs[16];
    int m_PC;
};

<CRegister.cpp>

#include "CRegister.h"

void C16RegisterFile::show_regs(){
    cout << "------ register file ------" << endl;
    
    for(int i = 0; i<16; i++){
        cout <<"REG"<<i<<": "<< read_from_reg(i)<<endl;
    }
}

```

## Main.cpp
```c++
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "CCode.h"
#include "CDecode.h"
#include "CExecute.h"
#include "CRegister.h"
#include "CMemory.h"

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 3){
		cout << "tpu <input file><line> \n";
		return -1;
	}
	CFlash1KWord code_memory(argv[1], atoi(argv[2]));
	for(int i=0; i<atoi(argv[2]); i++){
		cout << code_memory.code_at(i) << endl;
	}
	
	CT1DecodeDirectFetch decode(code_memory);
    C16RegisterFile      regs;
    CSRAM_256w           mems;

    CT1ExecuteTinyUnit   execute(decode, regs, mems);
    
    int size = atoi(argv[2]);
	int total_clks = 0;
    //for(int i=0; i<atoi(argv[2]); i++){
	while(regs.get_PC() < size){    
        decode.do_fetch_from(regs.get_PC());
        //decode.do_fetch_from(i);
		decode.do_decode();
		decode.show_instruction();
        total_clks += execute.do_execute();
    }

       cout<<"After excuting instruction ... " << endl;
       cout<<"Total clocks: " << total_clks << endl;
       regs.show_regs();
       mems.show_mems(0,74);
       
}

```


## Image of Execution
 

