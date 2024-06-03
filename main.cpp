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
	init_all();
	//position x;
	//char fen[] = "rn1q1bnr/ppp1kppp/8/1B1pp3/8/2N1Pb2/PPPP1PPP/1RBQK2R w K - 0 6";
	//ApplyFen(&x, fen);
	//move _move = {4, 6, king, short_castle_w, WHITE};
	//move _move = {8, 16, pawn, only_move, WHITE};
	//ReadableBoard(x);
	//make_move(&x, _move);
	//ReadableBoard(x);
	//undo_move(&x, _move);
	//ReadableBoard(x);
	//PrintBitBoard(x.wk);
	//for(int i =0; i < 15; i++) PrintBitBoard(*(&x.occupied + i));

/*	char str[] = "rn3bnr/pppqk1pp/8/1B1pppP1/8/2N1Pb2/PPPP1P1P/1RBQK2R w K f6 0 8";
	position x;
	ApplyFen(&x, str);
*/
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
