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

 ### **CCode** 

