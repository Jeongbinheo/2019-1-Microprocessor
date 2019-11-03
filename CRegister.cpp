#include "CRegister.h"

void C16RegisterFile::show_regs(){
    cout << "------ register file ------" << endl;
    
    for(int i = 0; i<16; i++){
        cout <<"REG"<<i<<": "<< read_from_reg(i)<<endl;
    }
}
