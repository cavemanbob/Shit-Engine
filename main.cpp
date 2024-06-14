#include "func.h"


/*
void printRemainingStack() {
    CONTEXT context;
    GetThreadContext(GetCurrentThread(), &context);

    // On x86, ESP register holds the stack pointer
    DWORD stackPointer = context.Rsp;
r1bq1rk1/pp1n1ppp/2pbpn2/3p4/7Q/2NBP3/PPPP1PPP/R1B1K1NR w KQ - 8 8

    // Print remaining stack space
    std::cout << "Remaining stack: " << stackPointer << " bytes\n";
}
*/

// FIX ZORBIST CAL
int main(){
	//test_case1();2kr3r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQ - 0 2
	
	//for(int i = 0; i < 64; i++){
		//PrintBitBoard(Zorbist[5][i]);1k1r1b1r/1ppbqp1p/4p3/Q2P3p/4B3/4PN2/PP1BKPPP/R6R w - - 0 19
	//}
/*	init_all();
	printf("WHITE\n");
	for(int i = 7; i >= 0; i--){
		for(int j = 0; j < 8; j++){
			printf("%4d ", eg_table[pawn][i * 8 + j]);
		}
		printf("\n");
	}
	printf("BLACK\n");
	for(int i = 7; i >= 0; i--){
		for(int j = 0; j < 8; j++){
			printf("%4d ", eg_table[pawn + 6][i * 8 + j]);
		}
		printf("\n");
	}*/
	Uci();


	return 0;

}
