


inline int is_square_attacked(position *b, u8 sqi, u8 side){ //defender side
	const u64 sq = 1ULL << sqi;
	const u8 piece_buffer = (side == WHITE) ? 6 : 0;
	const u64 occupied = b->occupied[WHITE] | b->occupied[BLACK];
	if(side == WHITE){ 
		if(((~AFILEMASK & b->bitboards[pawn + 6]) >> 9 | (~HFILEMASK & b->bitboards[pawn + 6]) >> 7) & sq) return 1;
	}
	else{
		if(((~HFILEMASK & b->bitboards[pawn]) << 9 | (~AFILEMASK & b->bitboards[pawn]) << 7) & sq) return 1;
	}
	if(GetRookWay(RelevantRookMask[sqi] & occupied, sqi) & (b->bitboards[rook + piece_buffer] | b->bitboards[queen + piece_buffer])) 
		return 1;
	if(GetBishopWay(RelevantBishopMask[sqi] & occupied, sqi) & (b->bitboards[bishop + piece_buffer] | b->bitboards[queen + piece_buffer]))
		return 1;
	if(Pnk_attacks[2][sqi] & b->bitboards[knight + piece_buffer])
		return 1;
	if(Pnk_attacks[3][sqi] & b->bitboards[king + piece_buffer])
		return 1;
	
	return 0;
}
inline int square_attacked_times(position *b, u8 sqi, u8 side){ //defender side
	const u64 sq = 1ULL << sqi;
	const int piece_buffer = side == WHITE ? 6 : 0;
	const u64 occupied = b->occupied[WHITE] | b->occupied[BLACK];
	u8 times = 0;
	if(side == WHITE){ 
		if(((~AFILEMASK & b->bitboards[pawn + 6]) >> 9 | (~HFILEMASK & b->bitboards[pawn + 6]) >> 7) & sq) times++;
	}
	else{
		if(((~HFILEMASK & b->bitboards[pawn]) << 9 | (~AFILEMASK & b->bitboards[pawn]) << 7) & sq) times++;
	}
	
	times += bit_count(GetRookWay(RelevantRookMask[sqi] & occupied, sqi) & (b->bitboards[rook + piece_buffer] | b->bitboards[queen + piece_buffer]));
	times += bit_count(GetBishopWay(RelevantBishopMask[sqi] & occupied, sqi) & (b->bitboards[bishop + piece_buffer] | b->bitboards[queen + piece_buffer])); 
	if(Pnk_attacks[2][sqi] & b->bitboards[knight + piece_buffer]) 
		times++;
	if(Pnk_attacks[3][sqi] & b->bitboards[king + piece_buffer]) 
		times++;
	
	return times;

}




void movegen(moves *r, position b){
	const u8 side = b.turn;
	//note use pawn pushes not in while
	const u64 pcw = 255ULL << 16;
	const u64 pcb = 255ULL << 40;
	const u8 eps = b.enpass_sq;
	const int eps_buffer = (side == WHITE) ? 8 : -8;
	position enpass_pos;
	if(eps != NO_SQUARE)enpass_pos = b;
	*r = {};
	const u64 empty = ~(b.occupied[WHITE] | b.occupied[BLACK]);
	const u64 emptyW = ~b.occupied[WHITE];
	const u64 emptyB = ~b.occupied[BLACK];
	const u64 EnemyPieces = b.occupied[1 - side];
	const u64 AllyPieces = b.occupied[side];
	const u64 occupied = b.occupied[WHITE] | b.occupied[BLACK];
	const u64 EnemySide = 1 - side;
	//if(b.wk == 0 || b.bk == 0){return r;}
	//u64 targets = side == WHITE ? b.woccupied : b.boccupied;
	// delete pinned pieces from targets
	// PIN FINDER
	const u8 piece_buffer = side == WHITE ? 0 : 6;
	const u8 king_square = lsb(b.bitboards[king + piece_buffer]);
	u64 King_PinMask = 0ULL;
	u64 CheckMask = ~0ULL; // if u are on the check this is change and u cant go a dangerous square
	u64 AntiCheckMask = ~0ULL;
	u64 QueenPinned = 0;
	//const u64 KingRelevantHV = RelevantRookMask[king_square];
	const u64 KingRelevantHV = GetRookWay(RelevantRookMask[king_square] & EnemyPieces, king_square);
	//const u64 KingRelevantD = RelevantBishopMask[king_square];
	const u64 KingRelevantD = GetBishopWay(RelevantBishopMask[king_square] & EnemyPieces, king_square);
	const u64 KingRelevant = KingRelevantD | KingRelevantHV;
	const u64 KingFullRelevantHV = FullRelevantRookMask[king_square];
	const u64 KingFullRelevantD = FullRelevantBishopMask[king_square];
	//alternative way in runtime ############################
	//const u64 KingRelevantHV = GetRookWay(RelevantRookMask[king_square], king_square);
	//const u64 KingRelevantD = GetRookWay(RelevantBishopMask[king_square], king_square);
	//#######################################################
	u64 EnemyThreats = ((GetRookWay(RelevantRookMask[king_square] & EnemyPieces, king_square) | GetBishopWay(RelevantBishopMask[king_square] & EnemyPieces, king_square)) & EnemyPieces);
	while(EnemyThreats){// for sliding pieces
		const int Current_Attacker = lsb(EnemyThreats);
		if(get_bit( (b.bitboards[queen + 6] | b.bitboards[rook + 6] | b.bitboards[queen] | b.bitboards[rook]) & KingFullRelevantHV , Current_Attacker)){ // is attacker rook or queen
			const u64 Attacker_Xray = GetRookWay(RelevantRookMask[Current_Attacker] & 1ULL << king_square, Current_Attacker); // get the squares until king (king included)
			if(bit_count(Attacker_Xray & KingRelevantHV & occupied) == 1){ // Are there any pieces between them
				King_PinMask |= (Attacker_Xray & KingRelevantHV) | 1ULL << Current_Attacker;
			}
			else if(bit_count(Attacker_Xray & KingRelevantHV & occupied) == 0){//ON THE CHECK
				CheckMask = 0ULL;
				CheckMask |= ~(Attacker_Xray & KingRelevantHV);
				AntiCheckMask = (Attacker_Xray & KingRelevantHV) | 1ULL << Current_Attacker;
			}
		}
		else if(get_bit( (b.bitboards[queen + 6] | b.bitboards[bishop + 6] | b.bitboards[queen] | b.bitboards[bishop]) & KingFullRelevantD, Current_Attacker)){ //diogonal check 
			const u64 Attacker_Xray = GetBishopWay(RelevantBishopMask[Current_Attacker] & 1ULL << king_square, Current_Attacker);
			if(bit_count(Attacker_Xray & KingRelevantD & occupied) == 1){ // Are there any pieces between them
				King_PinMask |= (Attacker_Xray & KingRelevantD) | 1ULL << Current_Attacker;
			}
			else if(bit_count(Attacker_Xray & KingRelevantD & occupied) == 0){//ON THE CHECK
				CheckMask = 0ULL;
				CheckMask |= ~(Attacker_Xray & KingRelevantD);
				AntiCheckMask = (Attacker_Xray & KingRelevantD) | 1ULL << Current_Attacker;
			}
		}
		EnemyThreats &= EnemyThreats - 1;
	}
	if(Pnk_attacks[2][king_square] & b.bitboards[knight + 6 - piece_buffer]){ // reversed side attacked by knight
		AntiCheckMask &= 1ULL << lsb(Pnk_attacks[2][king_square] & b.bitboards[knight + 6 - piece_buffer]);
	}
	if(side == WHITE){ // attacked by pawn
		if(king_square%8 != 7 && king_square/8 != 7 && 1ULL << king_square + 9 & b.bitboards[pawn + 6]) AntiCheckMask &= 1ULL << king_square + 9;	
		if(king_square%8 != 0 && king_square/8 != 7 && 1ULL << king_square + 7 & b.bitboards[pawn + 6]) AntiCheckMask &= 1ULL << king_square + 7;
	}
	else{
		if(king_square%8 != 0 && king_square/8 != 0 &&  1ULL << king_square - 9 & b.bitboards[pawn]) AntiCheckMask &= 1ULL << king_square - 9;	
		if(king_square%8 != 7 && king_square/8 != 0 &&  1ULL << king_square - 7 & b.bitboards[pawn]) AntiCheckMask &= 1ULL << king_square - 7;	
	}
	if(King_PinMask & b.bitboards[queen + piece_buffer]){
		QueenPinned = 1;
	}
	if(square_attacked_times(&b,king_square,side) > 1){// double check
		AntiCheckMask = 0ULL;
	}
	//PrintBitBoard(King_PinMask);
	//PrintBitBoard(CheckMask);
	//PrintBitBoard(AntiCheckMask);
	//PrintBitBoard(King_PinMask);
	//PrintBitBoard(occupied);
	//PrintBitBoard(KingRelevantHV)
	// core movegen
	u64 pawns   = b.bitboards[pawn + piece_buffer];
	u64 rooks   = b.bitboards[rook + piece_buffer];
	u64 knights = b.bitboards[knight + piece_buffer];
	u64 bishops = b.bitboards[bishop + piece_buffer];
	u64 queens  = b.bitboards[queen + piece_buffer];
	u64 kings   = b.bitboards[king + piece_buffer];
	u64 ways;
	const u64 ally_empty = ~b.occupied[side];
	//note have to add checkmask which is on the check blocking and anticheckmask !!

	//king
	// delete king for is_square_attacked()
	ways = Pnk_attacks[3][king_square] & ally_empty & CheckMask;
	pop_bit(b.occupied[side], king_square);
	pop_bit(b.bitboards[king + piece_buffer], king_square);
	while(ways){
		const u8 sqi = lsb(ways);
		const u64 sq = 1ULL << sqi;
		if(is_square_attacked(&b,sqi,side) == 0){
			move new_move = {king_square, sqi, king, only_move, side};
			moves_add(r, new_move);
		}
		ways &= ways - 1;
	}
	set_bit(b.occupied[side], king_square);
	set_bit(b.bitboards[king + piece_buffer], king_square);

	//rook
	while(rooks){
		const u8 sqi = lsb(rooks);
		ways = GetRookWay(occupied & RelevantRookMask[sqi], sqi) & ally_empty;
		if(King_PinMask & 1ULL << sqi){
			if(1ULL << sqi & FullRelevantBishopMask[king_square])
				ways &= FullRelevantBishopMask[king_square] & FullRelevantBishopMask[sqi];
			else
				ways &= FullRelevantRookMask[king_square] & FullRelevantRookMask[sqi];
		}
		ways &= AntiCheckMask;
		while(ways){
			move new_move = {sqi, lsb(ways), rook, only_move, side};
			moves_add(r, new_move);
			ways &= ways - 1;
		}
		rooks &= rooks - 1;
	}
	//bishop
	while(bishops){
		const u8 sqi = lsb(bishops);
		ways = GetBishopWay(occupied & RelevantBishopMask[sqi], sqi) & ally_empty;
		if(King_PinMask & 1ULL << sqi){
			if(1ULL << sqi & FullRelevantBishopMask[king_square])
				ways &= FullRelevantBishopMask[king_square] & FullRelevantBishopMask[sqi];
			else
				ways &= FullRelevantRookMask[king_square] & FullRelevantRookMask[sqi];
		}
		ways &= AntiCheckMask;
		while(ways){
			move new_move = {sqi, lsb(ways), bishop, only_move, side};
			moves_add(r, new_move);
			ways &= ways - 1;
		}
		bishops &= bishops - 1;
	}
	
	//queen
	while(queens){
		const u8 sqi = lsb(queens);
		ways = GetRookWay(occupied & RelevantRookMask[sqi], sqi) & ally_empty;
		if(King_PinMask & 1ULL << sqi){
			if(1ULL << sqi & FullRelevantBishopMask[king_square])
				ways &= FullRelevantBishopMask[king_square] & FullRelevantBishopMask[sqi];
			else
				ways &= FullRelevantRookMask[king_square] & FullRelevantRookMask[sqi];
		}
		ways |= GetBishopWay(occupied & RelevantBishopMask[sqi], sqi) & ally_empty;
		if(King_PinMask & 1ULL << sqi){
			if(1ULL << sqi & FullRelevantBishopMask[king_square])
				ways &= FullRelevantBishopMask[king_square] & FullRelevantBishopMask[sqi];
			else
				ways &= FullRelevantRookMask[king_square] & FullRelevantRookMask[sqi];
		}
		ways &= AntiCheckMask;
		while(ways){
			move new_move = {sqi, lsb(ways), queen, only_move, side};
			moves_add(r, new_move);
			ways &= ways - 1;
		}
		queens &= queens - 1;
	}
	//knights
	while(knights){
		const u8 sqi = lsb(knights);
		ways = Pnk_attacks[2][sqi] & ally_empty;
		if(King_PinMask & 1ULL << sqi) ways = 0ULL;
		ways &= AntiCheckMask;
		while(ways){
			move new_move = {sqi, lsb(ways), knight, only_move, side};
			moves_add(r, new_move);
			ways &= ways - 1;
		}
		knights &= knights - 1;
	}
	//pawns
	
	u64 pawns_promo = (side == WHITE) ? pawns & (RANK8MASK >> 8) : pawns & (RANK1MASK << 8); 
	pawns ^= pawns_promo; //del promo pawns
	//PrintBitBoard(King_PinMask);
	//std::cout << "\n eps: " << (int)eps << std::endl;
	while(pawns){
		const u8 sqi = lsb(pawns);
		ways = (Pnk_attacks[1 - side][sqi] & EnemyPieces) | ((side == WHITE ? 1ULL << (sqi + 8): 1ULL << (sqi - 8)) & empty); //push and captures
		if(side == WHITE && sqi < 16 && get_bit(occupied, sqi + 8) == 0 && get_bit(occupied, sqi + 16) == 0 ) ways |= 1ULL << sqi + 16; //double push
		else if(sqi > 47 && get_bit(occupied, sqi - 8) == 0 && get_bit(occupied, sqi - 16) == 0 ) ways |= 1ULL << sqi - 16; //double push
		
		if(King_PinMask & 1ULL << sqi){//pinned pawns
			if(1ULL << sqi & KingRelevantD) ways &= KingRelevantD;
			else ways &= KingRelevantHV;
			ways &= King_PinMask;
		}
		ways &= AntiCheckMask;
		while(ways){
			move new_move = {sqi, lsb(ways), pawn, only_move, side};
			moves_add(r, new_move);
			ways &= ways - 1;
		}
		if(sqi + 1 + eps_buffer == eps && sqi%8 != 7){
				//std::cout << "HERE" << (int)sqi << std::endl;
			move enpass_move = {sqi, (u8)(eps), pawn, (u8)(2 - side), side};
			make_move(&enpass_pos, enpass_move);
			//std::cout << "\n attacked " << (int)is_square_attacked(&enpass_pos, king_square, side) << std::endl;
			if(is_square_attacked(&enpass_pos, king_square, side) == 0){
				moves_add(r, enpass_move);
			}
			undo_move(&enpass_pos, enpass_move);
		} //right enpass
		if(sqi - 1 + eps_buffer == eps && sqi%8 != 0){
			move enpass_move = {sqi, (u8)(eps), pawn, (u8)(2 - side), side};
			make_move(&enpass_pos, enpass_move);
			if(is_square_attacked(&enpass_pos, king_square, side) == 0)
				moves_add(r, enpass_move);
			undo_move(&enpass_pos, enpass_move);
		} //left enpass
		pawns &= pawns - 1;
	}
	while(pawns_promo){
		const u8 sqi = lsb(pawns_promo);
		ways = (Pnk_attacks[1 - side][sqi] & EnemyPieces) | ((side == WHITE ? 1ULL << (sqi + 8): 1ULL << (sqi - 8)) & empty); //push and captures
		if(King_PinMask & 1ULL << sqi){//pinned pawns
			if(1ULL << sqi & KingRelevantD) ways &= KingRelevantD;
			else ways &= KingRelevantHV;
		}
		if(King_PinMask & 1ULL << sqi){//pinned pawns
			if(1ULL << sqi & KingRelevantD) ways &= KingRelevantD;
			else ways &= KingRelevantHV;
			ways &= King_PinMask;
		}
		ways &= AntiCheckMask;
		while(ways){
			for(int promo_enum = 3; promo_enum < 7; promo_enum++){
				move new_move = {sqi, lsb(ways), pawn, (u8)promo_enum, side};
				moves_add(r, new_move);
			}
			ways &= ways - 1;
		}
		pawns_promo &= pawns_promo - 1;
	}
	//PrintBitBoard(CheckMask);
	//PrintBitBoard(AntiCheckMask);
	if(AntiCheckMask != ~0ULL) return; // u cant castle while on the check
	//castles
	if(side == BLACK && get_bit(b.castling, 0) && 
		get_bit(occupied, 57) == 0 && get_bit(occupied, 58) == 0 && get_bit(occupied, 59) == 0 && 
		is_square_attacked(&b, 58, side) == 0 && is_square_attacked(&b,59 , side) == 0 && get_bit(b.bitboards[rook + piece_buffer], 56)){
		move new_move = {e8, c8, king, long_castle_b, BLACK};
		moves_add(r, new_move);
	}//lb
	if(side == BLACK && get_bit(b.castling, 1) && 
		get_bit(occupied, 61) == 0 && get_bit(occupied, 62) == 0 && 
		is_square_attacked(&b, 61, side) == 0 && is_square_attacked(&b,62 , side) == 0 && get_bit(b.bitboards[rook + piece_buffer], 63)){
		move new_move = {60, 62, king, short_castle_b, BLACK};
		moves_add(r, new_move);
	}//sb
	if(side == WHITE && get_bit(b.castling, 2) && 
		get_bit(occupied, 1) == 0 && get_bit(occupied, 2) == 0 && get_bit(occupied, 3) == 0 && 
		is_square_attacked(&b, 2, side) == 0 && is_square_attacked(&b,3 , side) == 0 && get_bit(b.bitboards[rook + piece_buffer], 0)){
		move new_move = {4, 2, king, long_castle_w, WHITE};
		moves_add(r, new_move);
	}//lw
	if(side == WHITE && get_bit(b.castling, 3) && 
		get_bit(occupied, 5) == 0 && get_bit(occupied, 6) == 0 && 
		is_square_attacked(&b,5 , side) == 0 && is_square_attacked(&b,6 , side) == 0 && get_bit(b.bitboards[rook + piece_buffer], 7)){
		move new_move = {4, 6, king, short_castle_w, WHITE};
		moves_add(r, new_move);
	}//sw
	

/*
	if(move_type == short_castle_w){b->wr |= 1ULL << 7; b->wr &= 1ULL << 5;}
	else if(move_type == long_castle_w){b->wr |= 1ULL << 0; b->wr &= 1ULL << 3;}
	else if(move_type == short_castle_b){b->br |= 1ULL << 63; b->wr &= 1ULL << 61;}
	else if(move_type == long_castle_b){b->br |= 1ULL << 56; b->wr &= 1ULL << 59;}*/
}


/*
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
}*/
