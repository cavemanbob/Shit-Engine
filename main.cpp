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
	/*
	InitPawnAttacks();
	InitPawnPushes();
	InitKnightAttacks();
	InitKingAttacks();
	InitRookAttacks();
	InitBishopAttacks();
	FindMagics(RelevantBishopMask, BishopMagics, BishopBase, BISHOP);
	FindMagics(RelevantRookMask, RookMagics, RookBase, ROOK);
	rand64();

	bitboard x = {};
	std::string fen = "rnbqkbnr/pp3ppp/8/N1pp4/4p3/8/PPPPPPPP/1RBQKBNR w Kkq - 0 5";
	FenApply(&x, fen);
	x = FromTo(x, 32, 49, KNIGHT_W);
	ReadableBoard(x);
	//for(int i=0; i < 15; i++) PrintBitBoard( *(&x.occupied + i));
	output m = movegen(x,BLACK);
	for(int i=0; i < m.from.size(); i++) {
		ReadableBoard(FromTo(x, m.from[i], m.to[i], m.PieceType[i]));
		std::cout << "\n";
	}
	std::cout << "S " << m.from.size() << std::endl;*/
	//GameLoop();
	Uci();
	return 0;
}
