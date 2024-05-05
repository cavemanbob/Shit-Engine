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
	InitPawnAttacks();
	InitPawnPushes();
	InitKnightAttacks();
	InitKingAttacks();
	InitRookAttacks();
	InitBishopAttacks();
	FindMagics(RelevantBishopMask, BishopMagics, BishopBase, BISHOP);
	FindMagics(RelevantRookMask, RookMagics, RookBase, ROOK);
	rand64();
	
	//std::clock_t start;
	bitboard x={};
	std::string fen = START_FEN;
	//std::string fen = "r3nrk1/pb1nq1bp/1p1p2p1/P1p1ppN1/4PP2/1PPP2P1/2Q3BP/R1B1RNK1 b - - 0 14";
	//std::string fen = "r3nrk1/pb1nq1bp/1P1pN1p1/2p1PP2/1PPP2PP/5B2/RB1N1K2/3Q4 w - - 0 1";
	//int square = 2;
	FenApply(&x,fen);
	std::cout << "Started Calculation\n";
	std::cout << BasicTree(x, WHITE, 3) << std::endl;

	//TestMoveGen();



	//output m = movegen(x, WHITE);
	//for(int i=0; i<m.to.size(); i++){
		//ReadableBoard(FromTo(x, m.from[i], m.to[i], m.PieceType[i]));
	//}
	//std::cout << m.to.size();
	//PrintBitBoard(GetBishopWay(RelevantBishopMask[square] & x.occupied, square));
/*	output m;
	int id;
	int y = 0;
	//std::string str;
	while(y < 150){
		y++;
		m = movegen(x,WHITE);
	  	id = rand64() % m.from.size();
		x = FromTo(x, m.from[id], m.to[id], m.PieceType[id]);
		std::cout << BitCount(x.occupied) << std::endl;
		ReadableBoard(x);
		//printRemainingStack();
		//_sleep(1000);
		//std::cin >> str;
		//x = PlayerMove(x,str);

	}*/
	//for(int i=0; i<m.from.size(); i++)if(m.from[i] == 34) ReadableBoard(FromTo(x, m.from[i], m.to[i], m.PieceType[i]));
	//std::cout << m.from.size();
	//for(int i=0; i<64; i++)PrintBitBoard(RelevantBishopMask[i]);
	//std::clock_t start;
	//start = std::clock();
	//std::cout << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << std::endl;
	//PrintBitBoard(RelevantBishopMask[square]);
/*	double max = 20.0;
	u64 stateseed=0;
	for(int i=0; i<180; i++){
		start = std::clock();
		u64 tempseed = state;
		FindRookMagics();
		double dif = ( std::clock() - start ) / (double) CLOCKS_PER_SEC ;
		if(dif < max){
			max = dif;
			stateseed = tempseed;
		}
		std::cout << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << "  " << stateseed << std::endl;	
		state = rand64();
	}
	std::cout << stateseed << std::endl << max;
*/

	return 0;
}
