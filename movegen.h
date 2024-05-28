


inline int is_square_attacked(position b, u8 sqi){
	int side = b.key >> 8 & 1ULL;
	const u64 sq = 1ULL << sqi;
	const int Piece_Buffer = (side == WHITE) ? 6 : 0;
	//std::cout << "Piece_Buffet " << Piece_Buffer << std::endl;
	if(side == WHITE){ 
		if(((~AFILEMASK & b.bp) >> 9 | (~HFILEMASK & b.bp) >> 7) & sq) return 1;
	}
	else{
		if(((~HFILEMASK & b.wp) << 9 | (~AFILEMASK & b.wp) << 7) & sq) return 1;
	}
	if(GetRookWay(RelevantRookMask[sqi] & b.occupied, sqi) & (*(&b.wr + Piece_Buffer) | *(&b.wq + Piece_Buffer))) 
		return 1;
	if(GetBishopWay(RelevantBishopMask[sqi] & b.occupied, sqi) & (*(&b.wb + Piece_Buffer) | *(&b.wq + Piece_Buffer))) 
		return 1;
	if(Pnk_attacks[2][sqi] & *(&b.wn + Piece_Buffer)) 
		return 1;
	if(Pnk_attacks[3][sqi] & *(&b.wk + Piece_Buffer)) 
		return 1;
	
	return 0;
}
inline int is_square_attacked(position b, u8 sqi, u8 side){
	const u64 sq = 1ULL << sqi;
	const int Piece_Buffer = (side == WHITE) ? 6 : 0;
	if(side == WHITE){ 
		if(((~AFILEMASK & b.bp) >> 9 | (~HFILEMASK & b.bp) >> 7) & sq) return 1;
	}
	else{
		if(((~HFILEMASK & b.wp) << 9 | (~AFILEMASK & b.wp) << 7) & sq) return 1;
	}
	if(GetRookWay(RelevantRookMask[sqi] & b.occupied, sqi) & (*(&b.wr + Piece_Buffer) | *(&b.wq + Piece_Buffer))) 
		return 1;
	if(GetBishopWay(RelevantBishopMask[sqi] & b.occupied, sqi) & (*(&b.wb + Piece_Buffer) | *(&b.wq + Piece_Buffer))) 
		return 1;
	if(Pnk_attacks[2][sqi] & *(&b.wn + Piece_Buffer)) 
		return 1;
	if(Pnk_attacks[3][sqi] & *(&b.wk + Piece_Buffer)) 
		return 1;
	
	return 0;
}
inline int square_attacked_times(position b, u8 sqi){
	const u8 side = b.key >> 8 & 1ULL;
	const u64 sq = 1ULL << sqi;
	const int Piece_Buffer = side == WHITE ? 6 : 0;
	u8 times = 0;
	if(side == WHITE){ 
		if(((~AFILEMASK & b.bp) >> 9 | (~HFILEMASK & b.bp) >> 7) & sq) times++;
	}
	else{
		if(((~HFILEMASK & b.wp) << 9 | (~AFILEMASK & b.wp) << 7) & sq) times++;
	}
	
	times += BitCount(GetRookWay(RelevantRookMask[sqi] & b.occupied, sqi) & (*(&b.wr + Piece_Buffer) | *(&b.wq + Piece_Buffer)));
	times += BitCount(GetBishopWay(RelevantBishopMask[sqi] & b.occupied, sqi) & (*(&b.wb + Piece_Buffer) | *(&b.wq + Piece_Buffer))); 
	if(Pnk_attacks[2][sqi] & *(&b.wn + Piece_Buffer)) 
		times++;
	if(Pnk_attacks[3][sqi] & *(&b.wk + Piece_Buffer)) 
		times++;
	
	return times;

}

// idk which one should come first 
position FromTo(position b, move x);

position FromTo(position b, u8 from, u8 to, u8 piece){
	//const u64 from = x.from;
	//const u64 to = x.to;
	//u64 piece = x.PieceType;
	//u64 side = piece < 6 ? WHITE : BLACK;
	u64 side = (b.key >> 8) & 1ULL;
	b.key ^= 1ULL << 8; //swap bit change turn
	
	if(piece == En_B){ // enpass
		b.occupied ^= 1ULL << to + 8;
		b.woccupied ^= 1ULL << to + 8;
		b.wp ^= 1ULL << to + 8;
		piece = PAWN_B;
	}
	else if(piece == En_W){ // enpass
		b.occupied ^= 1ULL << to - 8;
		b.boccupied ^= 1ULL << to - 8;
		b.bp ^= 1ULL << to - 8;
		piece = PAWN_W;
	}
	//promotion
	else if (piece > 13) {
		piece = piece - 14 + !side * 6;
		//std::cout << "piece " << piece << std::endl;
		//swap pawn to target piece
		*(&b.wp + !side * 6) ^= 1ULL << from;
		*(&b.wr + piece) |= 1ULL << from;
		b.key |= (piece - !side * 6 + 1) << 28; // +1 for str and no null
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
		//b.key |= 1ULL << 31;
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
	if(piece == ROOK_W && from == 0) b.key &= ~(1ULL << 11);
	else if(piece == ROOK_W && from == 7) b.key &= ~(1ULL << 12);
	else if(piece == ROOK_B && from == 56) b.key &= ~(1ULL << 9);
	else if(piece == ROOK_B && from == 63) b.key &= ~(1ULL << 10);
	else if(piece == KING_W) {b.key &= ~(1ULL << 12);b.key &= ~(1ULL << 11);}
	else if(piece == KING_B) {b.key &= ~(1ULL << 10);b.key &= ~(1ULL << 9);}
	//note add no castle when king moved (but check first is that need it)
	//enpass
	if((piece == PAWN_B || piece == PAWN_W) && abs((int)from / 8 - (int)to / 8) > 1 ) {b.key &= ~(0b1111111 << 20); b.key ^= to << 20;} else b.key &= ~(0b1111111 << 20); //enpassant set key
#ifdef DEBUG
	b.oldsquare = from;
#endif
	//b.to_square = to;
	return b;
}

position FromTo(position b, move x){
	const u8 from = x.from;
	const u8 to = x.to;
	u8 piece = x.PieceType;
	//u64 side = piece < 6 ? WHITE : BLACK;
	u8 side = (b.key >> 8) & 1ULL;
	b.key ^= 1ULL << 8; //swap bit change turn
	
	if(piece == En_B){ // enpass
		b.occupied ^= 1ULL << to + 8;
		b.woccupied ^= 1ULL << to + 8;
		b.wp ^= 1ULL << to + 8;
		piece = PAWN_B;
	}
	else if(piece == En_W){ // enpass
		b.occupied ^= 1ULL << to - 8;
		b.boccupied ^= 1ULL << to - 8;
		b.bp ^= 1ULL << to - 8;
		piece = PAWN_W;
	}
	//promotion
	else if (piece > 13) {
		piece = piece - 14 + !side * 6;
		//std::cout << "piece " << piece << std::endl;
		//swap pawn to target piece
		*(&b.wp + !side * 6) ^= 1ULL << from;
		*(&b.wr + piece) |= 1ULL << from;
		b.key |= (piece - !side * 6 + 1) << 28; // +1 for str and no null
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
		//b.key |= 1ULL << 31;
		//b.key |= 1ULL << 31; // capture flag
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
	if(piece == ROOK_W && from == 0) b.key &= ~(1ULL << 11);
	else if(piece == ROOK_W && from == 7) b.key &= ~(1ULL << 12);
	else if(piece == ROOK_B && from == 56) b.key &= ~(1ULL << 9);
	else if(piece == ROOK_B && from == 63) b.key &= ~(1ULL << 10);
	else if(piece == KING_W) {b.key &= ~(1ULL << 12);b.key &= ~(1ULL << 11);}
	else if(piece == KING_B) {b.key &= ~(1ULL << 10);b.key &= ~(1ULL << 9);}
	//note add no castle when king moved (but check first is that need it)
	//enpass
	if((piece == PAWN_B || piece == PAWN_W) && abs((int)from / 8 - (int)to / 8) > 1 ) {b.key &= ~(0b1111111 << 20); b.key ^= to << 20;} else b.key &= ~(0b1111111 << 20); //enpassant set key
#ifdef DEBUG
	b.oldsquare = from;
#endif
	//b.to_square = to;
	return b;
}



void movegen(Movelist *r, position b){// r must be empty or make them empty now
	int side = b.key >> 8 & 1ULL;
	//note use pawn pushes not in while
	u64 pcw = 255ULL << 16;
	u64 pcb = 255ULL << 40;
	const u8 eps = (b.key >> 20) & 0b1111111;
	*r = {};
	//Movelist *r = (Movelist *)malloc(sizeof(Movelist));
	const u64 empty = ~b.occupied;
	const u64 emptyW = ~b.woccupied;
	const u64 emptyB = ~b.boccupied;
	const u64 EnemyPieces = side == WHITE ? b.boccupied : b.woccupied;
	const u64 AllyPieces = side == WHITE ? b.woccupied : b.boccupied;
	const u64 EnemySide = side == WHITE ? BLACK : WHITE;
	//if(b.wk == 0 || b.bk == 0){return r;}
	u64 targets = side == WHITE ? b.woccupied : b.boccupied;
	// delete pinned pieces from targets
	// PIN FINDER
	const int Piece_Buffer = side == WHITE ? 0 : 6;
	const int King_Square = Ls1bIndex(*(&b.wk + Piece_Buffer));
	u64 King_PinMask = 0ULL;
	u64 CheckMask = ~0ULL;
	u64 AntiCheckMask = ~0ULL;
	u64 QueenPinned = 0;
	//const u64 KingRelevantHV = RelevantRookMask[King_Square];
	const u64 KingRelevantHV = GetRookWay(RelevantRookMask[King_Square] & EnemyPieces, King_Square);
	//const u64 KingRelevantD = RelevantBishopMask[King_Square];
	const u64 KingRelevantD = GetBishopWay(RelevantBishopMask[King_Square] & EnemyPieces, King_Square);
	const u64 KingRelevant = KingRelevantD | KingRelevantHV;
	const u64 KingFullRelevantHV = FullRelevantRookMask[King_Square];
	const u64 KingFullRelevantD = FullRelevantBishopMask[King_Square];
	//alternative way in runtime ############################
	//const u64 KingRelevantHV = GetRookWay(RelevantRookMask[King_Square], King_Square);
	//const u64 KingRelevantD = GetRookWay(RelevantBishopMask[King_Square], King_Square);
	//#######################################################
	u64 EnemyThreats = ((GetRookWay(RelevantRookMask[King_Square] & EnemyPieces, King_Square) | GetBishopWay(RelevantBishopMask[King_Square] & EnemyPieces, King_Square)) & EnemyPieces);
	while(EnemyThreats){// for sliding pieces
		const int Current_Attacker = Ls1bIndex(EnemyThreats);
		if(BitCheck( (b.bq | b.br | b.wq | b.wr) & KingFullRelevantHV , Current_Attacker)){ // is attacker rook or queen
			const u64 Attacker_Xray = GetRookWay(RelevantRookMask[Current_Attacker] & 1ULL << King_Square, Current_Attacker); // get the squares until king (king included)
			if(BitCount(Attacker_Xray & KingRelevantHV & b.occupied) == 1){ // Are there any pieces between them
				King_PinMask |= (Attacker_Xray & KingRelevantHV) | 1ULL << Current_Attacker;
			}
			else if(BitCount(Attacker_Xray & KingRelevantHV & b.occupied) == 0){//ON THE CHECK
				CheckMask = 0ULL;
				CheckMask |= ~(Attacker_Xray & KingRelevantHV);
				AntiCheckMask = (Attacker_Xray & KingRelevantHV) | 1ULL << Current_Attacker;
			}
		}
		else if(BitCheck( (b.bq | b.bb | b.wq | b.wb) & KingFullRelevantD, Current_Attacker)){ //diogonal check 
			const u64 Attacker_Xray = GetBishopWay(RelevantBishopMask[Current_Attacker] & 1ULL << King_Square, Current_Attacker);
			if(BitCount(Attacker_Xray & KingRelevantD & b.occupied) == 1){ // Are there any pieces between them
				King_PinMask |= (Attacker_Xray & KingRelevantD) | 1ULL << Current_Attacker;
			}
			else if(BitCount(Attacker_Xray & KingRelevantD & b.occupied) == 0){//ON THE CHECK
				CheckMask = 0ULL;
				CheckMask |= ~(Attacker_Xray & KingRelevantD);
				AntiCheckMask = (Attacker_Xray & KingRelevantD) | 1ULL << Current_Attacker;
			}
		}
		EnemyThreats &= EnemyThreats - 1;
	}
	
	if(Pnk_attacks[2][King_Square] & *(&b.bn - Piece_Buffer)){ // reversed side attacked by knight
		AntiCheckMask &= 1ULL << Ls1bIndex(Pnk_attacks[2][King_Square] & *(&b.bn - Piece_Buffer));
	}
	if(side == WHITE){ // attacked by pawn
		if(King_Square%8 != 7 && King_Square/8 != 7 && 1ULL << King_Square + 9 & b.bp) AntiCheckMask &= 1ULL << King_Square + 9;	
		if(King_Square%8 != 0 && King_Square/8 != 7 && 1ULL << King_Square + 7 & b.bp) AntiCheckMask &= 1ULL << King_Square + 7;
	}
	else{
		if(King_Square%8 != 0 && King_Square/8 != 0 &&  1ULL << King_Square - 9 & b.wp) AntiCheckMask &= 1ULL << King_Square - 9;	
		if(King_Square%8 != 7 && King_Square/8 != 0 &&  1ULL << King_Square - 7 & b.wp) AntiCheckMask &= 1ULL << King_Square - 7;	
	}
	if(King_PinMask & *(&b.wq + Piece_Buffer)){
		QueenPinned = 1;
	}
	if(square_attacked_times(b,King_Square) > 1){// double check
		AntiCheckMask = 0ULL;
	}
	// core movegen
	if(side == WHITE){
		position NoKingb = b;
		NoKingb.wk = 0ULL;
		NoKingb.occupied &= ~(1ULL << King_Square);
		while(targets){
			u8 i = Ls1bIndex(targets), j;
			for(j=0; j<6; j++)if(BitCheck(*(&b.wr + j),i))break;
			u64 ways = 0ULL;
			if(j == 5){ //pawn
				ways = (Pnk_attacks[0][i] & b.boccupied) | ( 1ULL << (i + 8) & ~b.occupied); //one push and attack
				if(i < 16 && BitCheck(b.occupied, i + 8) == 0 && BitCheck(b.occupied, i + 16) == 0 ) ways |= 1ULL << i + 16; //double push
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & KingRelevantD) ways &= KingRelevantD;
					else ways &= KingRelevantHV;
				}
				if(i + 1 == eps && i%8 != 7 && is_square_attacked(FromTo(b, i, eps + 8, En_W), King_Square, side) == 0){
					move k = {i, eps + 8, En_W};
					MoveList_Add(r, k);
				} //right enpass
				if(i - 1 == eps && i%8 != 0 && is_square_attacked(FromTo(b, i, eps + 8, En_W), King_Square, side) == 0){
					move k = {i, eps + 8, En_W};
					MoveList_Add(r, k);
				} //left enpass
				if(1ULL << i & King_PinMask)	ways &= King_PinMask;
				
				while(ways & RANK8MASK & AntiCheckMask){ //promotion moves
					const u8 pro_to = Ls1bIndex(ways & RANK8MASK & AntiCheckMask);
					for(u8 pro_type = 0; pro_type < 4; pro_type++){
						move k = {i, pro_to, pro_type + 14};
						MoveList_Add(r, k);
					}
					ways &= (ways & RANK8MASK & AntiCheckMask) - 1;
				}
			}
			else if(j == 1){ //knight
				ways = Pnk_attacks[2][i] & emptyW;
				if(King_PinMask & 1ULL << i) ways = 0ULL;
			}
			else if(j == 4){ //king
				ways = Pnk_attacks[3][i] & emptyW;
				//Cut moving to attacked square
				u64 King_Ways = ways;
				while(King_Ways){
					const u64 sqi = Ls1bIndex(King_Ways);
					const u64 sq = 1ULL << sqi;
					if(is_square_attacked(NoKingb,sqi))
						ways ^= sq;
					King_Ways &= King_Ways - 1;
				}
			}
			else if(j == 0){//rook
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			else if(j == 2){//bishop
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			else if(j == 3){//queen
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
				ways |= GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			if(1ULL << i & King_PinMask){
				ways &= King_PinMask;
			}
			if(j == KING_W)ways &= CheckMask; //CheckMove
			else ways &= AntiCheckMask;
			while(ways){
				move k = {i, Ls1bIndex(ways), j};
				MoveList_Add(r, k);
				ways &= ways - 1;

			}
			targets &= targets - 1;
		}
	}
	else{
		position NoKingb = b;
		NoKingb.bk = 0ULL;
		NoKingb.occupied &= ~(1ULL << King_Square);
		while(targets){
			u8 i = Ls1bIndex(targets), j;
			u64 ways = 0ULL;
			for(j=6; j<12; j++)if(BitCheck(*(&b.wr + j),i))break;
			if(j == 11){ //pawn
				ways = (Pnk_attacks[1][i] & b.woccupied) | ( 1ULL << (i - 8) & ~b.occupied); //push
				if(i > 47 && BitCheck(b.occupied, i - 8) == 0 && BitCheck(b.occupied, i - 16) == 0 ) ways |= 1ULL << i - 16; //double push
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & KingRelevantD) ways &= KingRelevantD;
					else ways &= KingRelevantHV;
				}
				if(i + 1 == eps && i%8 != 7 && is_square_attacked(FromTo(b, i, eps - 8, En_B), King_Square, side) == 0){
					move k = {i, eps - 8, En_B};
					MoveList_Add(r, k);
				} //right enpass
				if(i - 1 == eps && i%8 != 0 && is_square_attacked(FromTo(b, i, eps - 8, En_B), King_Square, side) == 0){
					move k = {i, eps - 8, En_B};
					MoveList_Add(r, k);
					} //left enpass
				if(1ULL << i & King_PinMask)	ways &= King_PinMask;
				
				while(ways & RANK1MASK & AntiCheckMask){ //promotion moves
					const u8 pro_to = Ls1bIndex(ways & RANK1MASK & AntiCheckMask);
					for(u8 pro_type = 0; pro_type < 4; pro_type++){
						move k = {i, pro_to, 14 + pro_type};
						MoveList_Add(r, k);
					}
					ways &= (ways & RANK1MASK & AntiCheckMask) - 1;
				}
			}
			else if(j == 7){ //knight
				ways = Pnk_attacks[2][i] & ~b.boccupied;
				if(King_PinMask & 1ULL << i) ways = 0ULL;
			}
			else if(j == 10){ //king
				ways = Pnk_attacks[3][i] & ~b.boccupied;
				//Cut moving to attacked square
				u64 King_Ways = ways;
				while(King_Ways){
					const u64 sqi = Ls1bIndex(King_Ways);
					const u64 sq = 1ULL << sqi;
					if(is_square_attacked(NoKingb,sqi))
						ways ^= sq;
					King_Ways &= King_Ways - 1;
				}
			}
			else if(j == 6){//rook
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			else if(j == 8){//bishop
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			else if(j == 9){//queen
				ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
				ways |= GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
				if(King_PinMask & 1ULL << i){
					if(1ULL << i & FullRelevantBishopMask[King_Square])
						ways &= FullRelevantBishopMask[King_Square] & FullRelevantBishopMask[i];
					else
						ways &= FullRelevantRookMask[King_Square] & FullRelevantRookMask[i];
				}
			}
			//CheckMove
			if(j == KING_B)ways &= CheckMask;
			else ways &= AntiCheckMask;
			
			while(ways){
				move k = {i, Ls1bIndex(ways), j};
				MoveList_Add(r, k);
				ways &= ways - 1;
			}
			targets &= targets - 1;
		}
	}
	//castle
	if(BitCheck(b.key, 12) && BitCheck(b.occupied,5) == 0 && BitCheck(b.occupied,6) == 0 && BitCheck(b.wr,7) && BitCheck(b.wk,4) && side == WHITE && is_square_attacked(b,5) == 0 && is_square_attacked(b,6) == 0 && is_square_attacked(b,4) == 0){
		move k = {4, 6, KING_W};
		MoveList_Add(r, k);
	} //S W
	if(BitCheck(b.key, 11) && BitCheck(b.occupied,1) == 0 && BitCheck(b.occupied,2) == 0 && BitCheck(b.occupied, 3) == 0 && BitCheck(b.wr,0) && BitCheck(b.wk,4) && side == WHITE && is_square_attacked(b,2) == 0 && is_square_attacked(b,3) == 0 && is_square_attacked(b,4) == 0){
		move k = {4, 2, KING_W};
		MoveList_Add(r, k);
	} //L W
	if(BitCheck(b.key, 10) && BitCheck(b.occupied,61) == 0 && BitCheck(b.occupied,62) == 0 && BitCheck(b.br,63) && BitCheck(b.bk,60) && side == BLACK && is_square_attacked(b,61) == 0 && is_square_attacked(b,62) == 0 && is_square_attacked(b,60) == 0){
		move k = {60, 62, KING_B};
		MoveList_Add(r, k);
	} //S B
	if(BitCheck(b.key, 9) && BitCheck(b.occupied,57) == 0 && BitCheck(b.occupied,58) == 0 && BitCheck(b.occupied, 59) == 0 && BitCheck(b.br,56) && BitCheck(b.bk,60) && side == BLACK && is_square_attacked(b,58) == 0 && is_square_attacked(b,59) == 0 && is_square_attacked(b,60) == 0){
		move k = {60, 58, KING_B};
		MoveList_Add(r, k);
	} //L B
	//return r;
}



int OnCheck(position b){ //reversed side (who do move, for other side is check control)
	int side = b.key >> 8 & 1ULL;
	side ^= 1ULL; //swap
	int King_Captured = 0;
	Movelist  m;
	movegen(&m, b);
	for(int i=0; i < m.Stack_size; i++){
		if(side == WHITE){
			if(FromTo(b, m.list[i]).wk == 0){
				King_Captured = 1;
				break;
			}
		}
		else{
			if(FromTo(b, m.list[i]).bk == 0){
				King_Captured = 1;
				break;
			}
		}
	}
	return King_Captured;
}
