all:
	g++ -o tpu CCode.cpp CDecode.cpp CExecute.cpp CRegister.cpp CMemory.cpp main.cpp
	./tpu 5matrix_revise.bin 405
	./tpu 5matrix_revise3.bin 389
