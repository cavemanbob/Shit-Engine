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



int main(){
/*	init_all();
	position x;
	char fen[] = "rn1q1bnr/ppp1kppp/8/1B1pp3/6b1/2N1PN2/PPPP1PPP/1RBQK2R w K - 2 5";
	ApplyFen(&x, fen);
	std::cout << perft(x, 2);*/
	//ReadableBoard(x);
	//for(int i = 0; i < 15; i++){
		//PrintBitBoard(*(&x.occupied + i));
	//}
/*	moves l;
	movegen(&l, x);
	for(int i = 0; i < l.size; i++){
		position k = x;
		make_move(&k, l.moves[i]);
		ReadableBoard(k);
		std::cout << ctos(l.moves[i].from).append(ctos(l.moves[i].to)) << (l.moves[i].move_type > 2 ? Promoting_str[l.moves[i].move_type - 2]: ' ') << " - 1" << std::endl;
	}
	std::cout << l.size << std::endl;
*/
	Uci();

	return 0;
}
