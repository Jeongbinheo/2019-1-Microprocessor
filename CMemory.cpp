#include "CMemory.h"

void CSRAM_256w :: show_mems(unsigned int start_addr, unsigned int end_addr){

    
    cout<< "------ memory Dump (addr: " << start_addr << "~" << end_addr << " )" << endl;
    
    for(unsigned int i = start_addr; i<=end_addr; i++){
        cout << read_from_memory(i) << " ";
    }
    cout << endl; 
};



