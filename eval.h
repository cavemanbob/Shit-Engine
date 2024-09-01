/*#include "c_nn.h"

nn *n1;
matrix *in, *out, *target;

void set_nn(){
	int scheme[4] = {14 * 64 + 64 + 8, 240, 48, 1};
	n1 = create_nn(4, scheme, sigmoid, 0.01f, 0.02f);
	in = create_matrix(14 * 64 + 64 + 8, 1);
	out = create_matrix(1, 1);
	target = create_matrix(1, 1);
}

void set_nn_in(position *b){
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 64; j++){
			in->data[i * 64 +j] = (float) get_bit(b->occupied[i], j);
		}
	}
	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 64; j++){
			in->data[2 * 64 + i * 64 +j] = (float) get_bit(b->bitboards[i], j);
		}
	}
	in->data[14 * 64] = b->enpass_sq ? 1 : 0;
	for(int i = 0; i < 8; i++) in->data[14 * 64 + 64 + i] = (float) get_bit(b->castling, i);
}

int eval_nn(position *b){
	set_nn_in(b);
	//printf("%f\n", feed(n1, in)->data[0]);
	return (int) ( feed(n1, in)->data[0] * 1000 ) - 500;
}*/

int Evaluate(position *b){
	int mg[2] = {}; // black 0 white 1
	int eg[2] = {};
	u64 woccupied = b->occupied[WHITE];
	u64 boccupied = b->occupied[BLACK];

	int GamePhase = 0;
	
	while(woccupied){
		int i, lsi = lsb(woccupied);
		for(i=0; i < 6; i++) if(get_bit(b->bitboards[i],lsi))break;
		mg[WHITE] += mg_table[i][lsi];
		eg[WHITE] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
		woccupied &= woccupied - 1;
	}		
	while(boccupied){
		int i, lsi = lsb(boccupied);
		for(i=0; i < 6; i++) if(get_bit(b->bitboards[6 + i],lsi))break;
		mg[BLACK] += mg_table[i + 6][lsi];
		eg[BLACK] += eg_table[i + 6][lsi];
		GamePhase += gamephaseInc[i];
		boccupied &= boccupied - 1;
	}	

	int mgScore = mg[WHITE] - mg[BLACK];
	int egScore = eg[WHITE] - eg[BLACK];
	//printf("\n white_eg_power %d\n black_eg_power %d\n", eg[WHITE], eg[BLACK]);
	int mgPhase = GamePhase;
	if(mgPhase > 24) mgPhase = 24; // promotion
	int egPhase = 24 - mgPhase;
	
/*	//double pawn
	const u64 white_pawns = b->bitboards[pawn];
	const u64 black_pawns = b->bitboards[pawn + 6];
	
	mgScore -= (bit_count(white_pawns | white_pawns << 8) - bit_count(black_pawns | black_pawns << 8)) * 20;
	egScore -= (bit_count(white_pawns | white_pawns << 8) - bit_count(black_pawns | black_pawns << 8)) * 50;

	//late game material is more cruial
	if(egPhase > mgPhase + 3){
		//printf("%d\n", egPhase);
		mgScore += (bit_count(b->occupied[WHITE] ^ b->bitboards[pawn]) - bit_count(b->occupied[BLACK] ^ b->bitboards[pawn + 6])) * 300;
		//printf("%d\n", (bit_count(b->occupied[WHITE] ^ b->bitboards[pawn]) - bit_count(b->occupied[BLACK] ^ b->bitboards[pawn + 6])) * 30000);
	}*/

	return (mgScore * mgPhase + egScore * egPhase) / 24;
}






