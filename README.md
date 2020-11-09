# 2019-1-Microprocessor
> ## Introduction
2019SS ELEC323 마이크로프로세서 수업에서 배운 Mini CPU(이하 tPU)를 C++ 언어를
이용해 구현한 코드들에 대한 설명입니다.

CPU, RAM, M0EMORY의 기능과 동작을 이해하고, 교수님께서 수업을 진행하면서 
가르쳐주신 코드임을 먼저 밝혀드립니다.

	
> ## tPU
 
tPU는 Tiny Process Unit의 약자로, 크게 CDecode, CExecute, and CRegsiter 3부분으로 분류될 수 있습니다. 

또한, tPU의 동작을 완전하게 설명하기 위해서는 CMemory와 CCode라는 부분도 추가적으로 필요합니다.


>## Structure of tPU by Image
![structure](https://blogfiles.pstatic.net/MjAxOTExMDNfMzEg/MDAxNTcyNzYyNjkwOTA3.wLwguXI5r4-XB7BHpWT0FR598g-_hOhOz96T2oEXdRQg.BLTL2pKgojD6CwGj4BhjYXIunjdaFCKgG-0-a_qR3r8g.JPEG.hdh988/tPU.jpg?type=w2)

-----

> ## Composition
1. [CCode](README.md#CCode)

2. [CDecode](README.md#CDecode)

3. [CExecute](README.md#CExecute)

4. [CRegister](README.md#CRegister)

5. [CMemory](README.md#CMemory)

-----

  >> ### CCode
  
   CCode 는 tPU가 실행할 Binary code를 업로드하는 과정을 구현한 부분으로

   C++ 언어의 file i/o 문법을 사용하여 구현하였습니다.

   헤더파일에는 두가지 Class의 함수, 변수에 대해 기술되어 있고,

   .cpp파일에는 메모장파일(.bin)으로 부터 읽어온 Character를 숫자열로 변환하는 기능을 구현함. 
    
   #### `>>  Let's see the Code`  [Stack.c](https://github.com/Jeongbinheo/Datastructure/blob/master/stack.c) (클릭 시 전체 코드 페이지로 이동)

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

-----

 >>### CDecode
   
CCode를 통해 받아들인 비트열을 OPCODE, OPERAND1, OPERAND2로 분해하고 EXECUTION 단에 넘겨주는 역할을 하는 Decoder단을 

구현한 코드로 전체 코드는 길지만 bit masking하는 코드에 대해서 알아보자 
 
   ####  `>>  Let's see the Code`  
   #### [CDecode.cpp](https://github.com/Jeongbinheo/Microprocessor/blob/master/CDecode.cpp) (클릭 시 전체 코드 페이지로 이동)
   #### [CDecode.h](https://github.com/Jeongbinheo/Microprocessor/blob/master/CDecode.h) (클릭 시 전체 코드 페이지로 이동)
	 
```c++

typedef struct {
	unsigned int OPCODE : 4;
	unsigned int OP1    : 4;
		 int OP2    : 8;
} SInstruction;


    Execution Unit에 사용되는 Instruction Code는 struct로 다음과 같은 비트로 정의됨. 
      OPCODE : 4bit
      OPERAND1: 4bit
      OPERAND2: 8bit 

```


   #### **`Bit Masking ` 에 해당하는 함수코드**
       .bin 파일에서 불러온 비트배열의 각 자리수별로 OR연산(|=)을 하여 m_instruction의 필드인 OPCODE,OP1,OP2를 정의하는 과정을 보여줌

```c++

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
 -----
 
>> ### **CExecute**

 - Decode에서 OPCODE, OP1, OP2의 비트를 얻었고, OPCODE에 알맞은 동작을 실행시키는 과정을 담당하는 클래스
   
   ####  `>>  Let's see the Code`  
   #### [CExecute.cpp](https://github.com/Jeongbinheo/Microprocessor/blob/master/CExecute.cpp) (클릭 시 전체 코드 페이지로 이동)
   #### [CExecute.h](https://github.com/Jeongbinheo/Microprocessor/blob/master/CExecute.h) (클릭 시 전체 코드 페이지로 이동)


 - **`CExecute의 구조 및 함수`** 
 
```C++
 
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
    				: m_decode_unit(decode), m_regs(regs), m_mems(mems){ }
				
				//initiate private member value
    				// m_deocde_unit = decode;
    				// m_regs = regs;
				
    				virtual ~CT1ExecuteTinyUnit(){ }	

    				int do_execute();

			private:

    				CT1DecodeDirectFetch& m_decode_unit;
    				C16RegisterFile&      m_regs;
    				CSRAM_256w&           m_mems;

			};
```


   **→ CExecute 에는 OPCODE 16개에 해당하는 기능이 정의되어 있는데 그 중에서**
   
   - **MOV3** (OP1이 지정하는 Register에 OP2의 값을 넣는 기능),
   
   - **MUL** (OP1, OP2가 지정하는 Register 내부의 값을 곱하여 OP1에 저장) 
   
     **두 가지에 대해서 코드로 살펴보자.**
  
  `※CExecute 클래스에서는 m_decode, m_regs, m_mems의 함수도 같이 사용됨`
  
  
```c++
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

    		}else if(m_decode_unit.get_opcode() == MUL){
                 
			unsigned int reg_n = m_decode_unit.get_op1();
                     	unsigned int reg_m = (m_decode_unit.get_op2()>>4)& 0xf; //8비트 OPERAND2를 4비트로 마스킹하는과정.

                    	int Rn = m_regs.read_from_reg(reg_n);
                     	int Rm = m_regs.read_from_reg(reg_m);

                     	Rn = Rn * Rm;

                     	m_regs.write_on_reg(reg_n, Rn);

                     	m_regs.set_PC(m_regs.get_PC()+1);

                    	return clks[MUL];
                                                 
```

-----

### **CRegister** 
 - CPU의 Register 동작을 구현한 클래스로 Register내부의 데이터를 read, write하고, 
   PC(Program counter)값을 증가시키는 함수가 포함되어 있는 클래스
   
   ####  `>>  Let's see the Code`  
   #### [CExecute.cpp](https://github.com/Jeongbinheo/Microprocessor/blob/master/CExecute.cpp) (클릭 시 전체 코드 페이지로 이동)
   #### [CExecute.h](https://github.com/Jeongbinheo/Microprocessor/blob/master/CExecute.h) (클릭 시 전체 코드 페이지로 이동)
   
   
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
 

