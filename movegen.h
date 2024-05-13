



output movegen(bitboard b){
	int side = b.key >> 8 & 1ULL;
	//note use pawn pushes not in while
	u64 pcw = 255ULL << 16;
	u64 pcb = 255ULL << 40;
	u64 eps = (b.key >> 19) & 0b1111111;
	//std::cout << "eps " << eps << std::endl;
	output r = {};
	const u64 empty = ~b.occupied;
	const u64 emptyW = ~b.woccupied;
	const u64 emptyB = ~b.boccupied;
	const u64 EnemyPieces = side == WHITE ? b.boccupied : b.woccupied;
	const u64 AllyPieces = side == WHITE ? b.woccupied : b.boccupied;
	//if(b.wk == 0 || b.bk == 0){return r;}
	u64 targets = side == WHITE ? b.woccupied : b.boccupied;
	// delete pinned pieces from targets
	// PIN FINDER
	const int Piece_Buffer = side == WHITE ? 0 : 6;
	const int King_Square = Ls1bIndex(*(&b.wk + Piece_Buffer));
	u64 King_PinMask = 0ULL;
	const u64 KingRelevant = RelevantBishopMask[King_Square] | RelevantRookMask[King_Square];
	const u64 KingRelevantHV = RelevantRookMask[King_Square];
	const u64 KingRelevantD = RelevantBishopMask[King_Square];
	u64 EnemyThreats = ((GetRookWay(RelevantRookMask[King_Square] & EnemyPieces, King_Square) | GetBishopWay(RelevantBishopMask[King_Square] & EnemyPieces, King_Square)) & EnemyPieces);
	while(EnemyThreats){
		const int Current_Attacker = Ls1bIndex(EnemyThreats);
		if(BitCheck( b.bq | b.br | b.wq | b.wr , Current_Attacker)){ 
			const u64 Attacker_Xray = GetRookWay(RelevantRookMask[Current_Attacker] & 1ULL << King_Square, Current_Attacker);
			if(BitCount(Attacker_Xray & KingRelevantHV & b.occupied) == 1){ 
				King_PinMask |= Attacker_Xray & KingRelevantHV;
			}
		}
		else if(BitCheck( b.bq | b.bb | b.wq | b.wb , Current_Attacker)){ //diogonal check 
			const u64 Attacker_Xray = GetBishopWay(RelevantBishopMask[Current_Attacker] & 1ULL << King_Square, Current_Attacker);
			if(BitCount(Attacker_Xray & KingRelevantD & b.occupied) == 1){
				King_PinMask |= Attacker_Xray & KingRelevantD;
			}
		}
		EnemyThreats &= EnemyThreats - 1;
	}

	if(side == WHITE){
		while(targets){
			int i = Ls1bIndex(targets), j;
			for(j=0; j<6; j++)if(BitCheck(*(&b.wr + j),i))break;
			u64 ways = 0ULL;
			if(j == 5){ //pawn
				ways = (Pnk_attacks[0][i] & b.boccupied) | ( 1ULL << (i + 8) & ~b.occupied); //push
				if(i < 16 && BitCheck(b.occupied, i + 8) == 0 && BitCheck(b.occupied, i + 16) == 0 ) ways |= 1ULL << i + 16; //double push
				if(i + 1 == eps && i%8 != 7){ r.from.push_back(i); r.to.push_back(eps + 8); r.PieceType.push_back(En_W);} //right enpass
				if(i - 1 == eps && i%8 != 0){ r.from.push_back(i); r.to.push_back(eps + 8); r.PieceType.push_back(En_W);} //left enpass
			}
			else if(j == 1){ //knight
				ways = Pnk_attacks[2][i] & emptyW;
			}
			else if(j == 4){ //king
				ways = Pnk_attacks[3][i] & emptyW;
				//Cut moving to attacked square
				u64 King_Ways = ways;
				while(King_Ways){
					const u64 sqi = Ls1bIndex(King_Ways);
					const u64 sq = 1ULL << sqi;
					if(sq & (b.bp >> 9 | b.bp >> 7)){// attacked by pawn
						ways ^= sq;
					}
					else if(sq & Pnk_attacks[3][sqi]){// attacked by king
						ways ^= sq;
					}
					else if(sq & Pnk_attacks[2][sqi]){// attacked by knight
						ways ^= sq;
					}
					else if(sq & GetRookWay(RelevantRookMask[sqi] & b.occupied, sqi) & b.boccupied){ // rook and queen
						ways ^= sq;
					}
					else if(sq & GetBishopWay(RelevantBishopMask[sqi] & b.occupied, sqi) & b.boccupied){ // bishop and queen
						ways ^= sq;
					}
					King_Ways &= King_Ways - 1;
				}
			}
			else if(j == 0){//rook
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
			}
			else if(j == 2){//bishop
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
			}
			else if(j == 3){//queen
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
				ways |= GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
			}
			if(1ULL << i & King_PinMask){
				ways &= King_PinMask;
			}
			while(ways){
				r.from.push_back(i);
				r.to.push_back(Ls1bIndex(ways));
				ways &= ways - 1;
				r.PieceType.push_back(j);
			}
			targets &= targets - 1;
		}
	}
	else{
		while(targets){
			int i = Ls1bIndex(targets), j;
			u64 ways = 0ULL;
			for(j=6; j<12; j++)if(BitCheck(*(&b.wr + j),i))break;
			if(j == 11){ //pawn
				ways = (Pnk_attacks[1][i] & b.woccupied) | ( 1ULL << (i - 8) & ~b.occupied); //push
				if(i > 47 && BitCheck(b.occupied, i - 8) == 0 && BitCheck(b.occupied, i - 16) == 0 ) ways |= 1ULL << i - 16; //double push
				if(i + 1 == eps){ r.from.push_back(i); r.to.push_back(eps - 8); r.PieceType.push_back(En_B);} //right enpass
				if(i - 1 == eps){ r.from.push_back(i); r.to.push_back(eps - 8); r.PieceType.push_back(En_B);} //left enpass
			}
			else if(j == 7){ //knight
				ways = Pnk_attacks[2][i] & ~b.boccupied;
			}
			else if(j == 10){ //king
				ways = Pnk_attacks[3][i] & ~b.boccupied;
				//Cut moving to attacked square
				u64 King_Ways = ways;
				while(King_Ways){
					const u64 sqi = Ls1bIndex(King_Ways);
					const u64 sq = 1ULL << sqi;
					if(sq & (b.wp << 9 | b.wp << 7)){// attacked by pawn
						ways ^= sq;
					}
					else if(sq & Pnk_attacks[3][sqi]){// attacked by king
						ways ^= sq;
					}
					else if(sq & Pnk_attacks[2][sqi]){// attacked by knight
						ways ^= sq;
					}
					else if(sq & GetRookWay(RelevantRookMask[sqi] & b.occupied, sqi) & b.woccupied){ // rook and queen
						ways ^= sq;
					}
					else if(sq & GetBishopWay(RelevantBishopMask[sqi] & b.occupied, sqi) & b.woccupied){ // bishop and queen
						ways ^= sq;
					}
					King_Ways &= King_Ways - 1;
				}
			}
			else if(j == 6){//rook
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
			}
			else if(j == 8){//bishop
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
			}
			else if(j == 9){//queen
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
				ways |= GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
			}
			if(1ULL << i & King_PinMask){
				ways &= King_PinMask;
			}
			while(ways){
				r.from.push_back(i);
				r.to.push_back(Ls1bIndex(ways));
				ways &= ways - 1;
				r.PieceType.push_back(j);
			}
			targets &= targets - 1;
		}
	}
	//castle
	if(BitCheck(b.key, 12) && BitCheck(b.occupied,5) == 0 && BitCheck(b.occupied,6) == 0 && BitCheck(b.wr,7) && BitCheck(b.wk,4)){
		r.from.push_back(4); r.to.push_back(6); r.PieceType.push_back(KING_W);
	} //S W
	if(BitCheck(b.key, 11) && BitCheck(b.occupied,1) == 0 && BitCheck(b.occupied,2) == 0 && BitCheck(b.occupied, 3) == 0 && BitCheck(b.wr,0) && BitCheck(b.wk,4)){
		r.from.push_back(4); r.to.push_back(2); r.PieceType.push_back(KING_W);
	} //L W
	if(BitCheck(b.key, 10) && BitCheck(b.occupied,61) == 0 && BitCheck(b.occupied,62) == 0 && BitCheck(b.wr,64) && BitCheck(b.wk,4)){
		r.from.push_back(60); r.to.push_back(62); r.PieceType.push_back(KING_W);
	} //S B
	if(BitCheck(b.key, 9) && BitCheck(b.occupied,57) == 0 && BitCheck(b.occupied,58) == 0 && BitCheck(b.occupied, 59) == 0 && BitCheck(b.wr,56) && BitCheck(b.wk,4)){
		r.from.push_back(60); r.to.push_back(58); r.PieceType.push_back(KING_W);
	} //L B
	
	return r;
}


bitboard FromTo(bitboard b, int from, int to, int piece){
	u64 side = piece < 6 ? WHITE : BLACK;
	b.key ^= 1ULL << 8; //swap bit
	
	if(piece == En_B){ // enpass
		b.occupied ^= 1ULL << to + 8;
		b.woccupied ^= 1ULL << to + 8;
		b.wp ^= 1ULL << to + 8;
		piece = PAWN_B;
	}
	else if(piece == En_W){ // enpass
		b.occupied ^= 1ULL << to - 8;
		b.woccupied ^= 1ULL << to - 8;
		b.wp ^= 1ULL << to - 8;
		piece = PAWN_W;
	}

	if(BitCheck(b.occupied, to) == 1){ // Capture
		if(side == WHITE) 
			b.boccupied ^= 1ULL << to;
		else 
			b.woccupied ^= 1ULL << to;

		if(side == WHITE){
			for(int i=0; i < 6; i++)
				if(BitCheck(*(&b.br + i), to) == 1){
					*(&b.br + i) ^= 1ULL << to;
					break;
				}
		}
		else{
			for(int i=0; i < 6; i++)
				if(BitCheck(*(&b.wr + i), to) == 1){
					*(&b.wr + i) ^= 1ULL << to;
					break;
				}
		}
	}
	else{ // move
		b.occupied ^= 1ULL << to;
	}

	b.occupied ^= 1ULL << from; 

	if(side == WHITE){
		b.woccupied ^= 1ULL << from; 
		b.woccupied ^= 1ULL << to;
	}
	else{
		b.boccupied ^= 1ULL << from; 
		b.boccupied ^= 1ULL << to;
	}
	*(&b.wr + piece) ^= 1ULL << from;
	*(&b.wr + piece) ^= 1ULL << to;


	if (piece == PAWN_W && to > 55) { //promotion
		*(&b.wr + piece) ^= 1ULL << to;
		*(&b.wr + QUEEN_W) ^= 1ULL << to;
	}	
	else if(piece == PAWN_B && to < 8) { //promotion
		*(&b.wr + piece) ^= 1ULL << to;
		*(&b.wr + QUEEN_B) ^= 1ULL << to;
	}
	//castle
	if(piece == KING_W && from == 4 && to == 6) {
		b.key ^= 1ULL << 8; //swap turn
		b = FromTo(b, 7, 5, ROOK_W);
	}
	else if(piece == KING_W && from == 4 && to == 2){
		b.key ^= 1ULL << 8;
		b = FromTo(b, 0, 3, ROOK_W);
	}
	else if(piece == KING_B && from == 60 && to == 62) {
		b.key ^= 1ULL << 8;
		b = FromTo(b, 63, 61, ROOK_B);
	}
	else if(piece == KING_B && from == 60 && to == 58){
		b.key ^= 1ULL << 8;
		b = FromTo(b, 56, 59, ROOK_B);
	}
	// no castle when rook moved
	if(piece == ROOK_W && from == 0) b.key &= ~(1ULL << 3);
	else if(piece == ROOK_W && from == 7) b.key &= ~(1ULL << 2);
	else if(piece == ROOK_B && from == 56) b.key &= ~(1ULL << 0);
	else if(piece == ROOK_B && from == 63) b.key &= ~(1ULL << 1);
	//enpass
	if((piece == PAWN_B || piece == PAWN_W) && abs(from / 8 - to / 8) > 1 ) {b.key &= ~(0b1111111 << 19); b.key ^= to << 19;} else b.key &= ~(0b1111111 << 19); //enpassant set key
#ifdef DEBUG
	b.oldsquare = from;
#endif
	return b;
}

int OnCheck(bitboard b){ //reversed side (who do move, for other side is check control)
	int side = b.key >> 8 & 1ULL;
	side ^= 1ULL; //swap
	int King_Captured = 0;
	output m = movegen(b);
	for(int i=0; i < m.from.size(); i++){
		if(side == WHITE){
			if(FromTo(b, m.from[i], m.to[i], m.PieceType[i]).wk == 0){
				King_Captured = 1;
				break;
			}
		}
		else{
			if(FromTo(b, m.from[i], m.to[i], m.PieceType[i]).bk == 0){
				King_Captured = 1;
				break;
			}
		}
	}
	return King_Captured;
}
