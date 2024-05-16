#include "func.h"


/*
void printRemainingStack() {
    CONTEXT context;
    GetThreadContext(GetCurrentThread(), &context);

    // On x86, ESP register holds the stack pointer
    DWORD stackPointer = context.Rsp;

    // Print remaining stack space
    std::cout << "Remaining stack: " << stackPointer << " bytes\n";
}
*/

int main(){/*
	bitboard x = {};
	//char str[] = "r3k3/p1ppqp2/bnN1pnp1/3P3r/1p2P3/5Q2/PPPBBPpP/R2NK2R b KQq - 3 5";
	char str[] = "1k6/8/rN3K2/3pP3/8/2b5/8/8 w - - 0 1";
	ApplyFen(&x,str);
	ReadableBoard(x);
	output m = movegen(x);
	for(int i =0; i < m.from.size(); i++){
		ReadableBoard(FromTo(x, m.from[i], m.to[i], m.PieceType[i]));
	}
	std::cout << "moves: " << m.from.size() << std::endl;*/
	Uci();
	//PrintBitBoard(~AFILEMASK & ~0ULL);
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
