#include "func.h"



void printRemainingStack() {
    CONTEXT context;
    GetThreadContext(GetCurrentThread(), &context);

    // On x86, ESP register holds the stack pointer
    DWORD stackPointer = context.Rsp;

    // Print remaining stack space
    std::cout << "Remaining stack: " << stackPointer << " bytes\n";
}


int main(){
	Uci();
	/*
	InitFullBishopAttacks();
	InitFullRookAttacks();
	for(int i = 0; i < 64; i++){
		PrintBitBoard(FullRelevantRookMask[i]);
		PrintBitBoard(FullRelevantBishopMask[i]);
		std::cout << i << "\n\n";
	}*/

	return 0;
}
