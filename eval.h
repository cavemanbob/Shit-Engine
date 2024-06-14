
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







