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
	//char fen[] = "rn3bnr/pppqk1pp/8/1B1pppP1/8/2N1Pb2/PPPP1P1P/1RBQK2R w K f6 0 8";
	//ApplyFen(&x, fen);
	//std::cout << "move1 king_move w" << std::endl;
/*	move move = {e1, f1, king, only_move, WHITE};
	make_move(&x, move);*/
/*	PrintBitBoard(x.occupied[0]);
	std::cout << "BLACK\n"; 
	PrintBitBoard(x.occupied[1]);
	std::cout << "WHITE\n";
	for(int i = 0; i < 12; i++){PrintBitBoard(x.bitboards[i]);std::cout << i << std::endl;}*/

	//undo_move(&x, move);

	
	Uci();
	init_all();
	position x;
	char str[] = "1r2k2r/p1ppqpb1/bn2p1N1/3P4/1p2n3/2N2Q2/PPPBBPpP/R1K4R w k - 0 5";
	ApplyFen(&x, str);
	//move _move = {e8, c8, king, long_castle_b, BLACK};
	move _move = {g6, h8, knight, only_move, WHITE};
	make_move(&x, _move);
	//undo_move(&x, _move);
	print_position_bitboards(&x);
	print_position_flags(&x);
	ReadableBoard(x);
	moves l;
	movegen(&l, x);
	std::cout << "possible move " << (int)l.size << std::endl;
	//std::cout << "TURN " << (int)x.turn << std::endl;
	//std::cout << "move: " << (int)l.moves[0].from << " " << (int)l.moves[0].to << " " <<
	  //(int)l.moves[0].moved_piece << " " << (int)l.moves[0].move_type << " " << (int)l.moves[0].side <<	std::endl;
	//move _move = {4, 5, king, only_move, WHITE};
	//for(int i = 0; i < l.size; i++){
		//make_move(&x, l.moves[i]);
		//undo_move(&x, l.moves[i]);
	//}
	//print_position_flags(&x);
/*	for(int i = 0; i < l.size; i++){
		std::cout << i;
		make_move(&x, l.moves[i]);
		undo_move(&x, l.moves[i]);*/
	/*	moves p;
		movegen(&p, x);
		if(p.size != 30)
			std::cout << "move -> " << (int)p.moves[i].from << " - " << (int)p.moves[i].to << " piece " << (int)p.moves[i].moved_piece
			<< "  move_type " << (int)p.moves[i].move_type << " side " << (int)p.moves[i].side << " size " << (int)p.size << std::endl;
	}*/
	//movegen(&l, x);
	//ReadableBoard(x);
	//std::cout << "posssible moves " << l.size << std::endl;
/*	std::cout << "move2 eps_white" << std::endl;
	move = {g5, f6, pawn, en_w, WHITE};
	make_move(&x, move);
	undo_move(&x, move);
	std::cout << "move3 capture white" << std::endl;
	move = {c1, d5, knight, only_move, WHITE};
	make_move(&x, move);
	undo_move(&x, move);
	std::cout << "move4 castling w" << std::endl;
	move = {e1, g1, king, short_castle_w, WHITE};
	make_move(&x, move);
	undo_move(&x, move);
	moves m;
	movegen(&m, x);
	std::cout << (int)m.size << std::endl;
	//PrintBitBoard(x.bitboards[6]);
	ReadableBoard(x);*/
	return 0;
}
