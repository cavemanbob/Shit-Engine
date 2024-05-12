

void InitPawnAttacks(){
	u64 b = 5, eb = 351843720888320ULL, c = 0xFF, ec = 0xFF0000000000;
	for(int i=8; i<56;i++){
		u64 CurrentAttack = (b << i + 7) & (c << ((i/8 + 1) * 8));
		Pnk_attacks[0][i] = CurrentAttack;
	}
	for(int i=0; i<56;i++){
		u64 CurrentAttack = (eb >> i) & (ec >> i/8 * 8);
		Pnk_attacks[1][55 - i] = CurrentAttack;
	}
}
void InitPawnPushes(){ // wrong
	u64 c = 1ULL, dc = 257ULL;
	for(int i=16; i<56; i++){
		p_pushes[0][i] = c << i + 8;
	}
	for(int i=8; i<16; i++){
		p_pushes[0][i] = dc << i + 8;
	}
	for(int i=8; i<48; i++){
		p_pushes[1][i] = c << i - 8;
	}
	for(int i=48; i<56; i++){
		p_pushes[1][i] = dc << i - 16;
	}
}

void InitKnightAttacks(){
	u64 empty = 0ULL;
	for(int i=0; i<64; i++){
		u64 c = empty;
		if(i%8 > 0 && i > 15) c |= 1ULL << i + SOUTH + SOUTH + WEST;
		if(i%8 > 1 && i > 7) c |= 1ULL << i + SOUTH + WEST + WEST;
		if(i%8 < 7 && i > 15) c |= 1ULL << i + SOUTH + SOUTH + EAST;
		if(i%8 < 6 && i > 7) c |= 1ULL << i + SOUTH + EAST + EAST;
		if(i%8 > 0 && i < 48) c |= 1ULL << i + NORTH + NORTH + WEST;
		if(i%8 > 1 && i < 56) c |= 1ULL << i + NORTH + WEST + WEST;
		if(i%8 < 7 && i < 48) c |= 1ULL << i + NORTH + NORTH + EAST;
		if(i%8 < 6 && i < 56) c |= 1ULL << i + NORTH + EAST + EAST;
    eg_pawn_table,
		Pnk_attacks[2][i] = c;
	}
}
void InitKingAttacks(){
	u64 empty = 0ULL;
	for(int i=0; i<64; i++){
		u64 c = empty;
		if(i%8 > 0) c |= 1ULL << i + WEST;
		if(i%8 < 7) c |= 1ULL << i + EAST;
		if(i > 7) c |= 1ULL << i + SOUTH;
		if(i < 56) c |= 1ULL << i + NORTH;
		if(i%8 > 0 && i > 7) c |= 1ULL << i + SOUTH  + WEST;
		if(i%8 < 7 && i > 7) c |= 1ULL << i + SOUTH  + EAST;
		if(i%8 > 0 && i < 56) c |= 1ULL << i + NORTH  + WEST;
		if(i%8 < 7 && i < 56) c |= 1ULL << i + NORTH  + EAST;
		Pnk_attacks[3][i] = c;
	}
}

void InitRookAttacks(){
	int dirlist[] = {NORTH, WEST, SOUTH, EAST };
	for(int i=0; i<64; i++){
		u64 c = 0ULL;
		for(int j=1; j<8; j++)
			if(i%8 > j) c |= 1ULL << i - j; else break;
		for(int j=1; j<8; j++)
			if(7 - i%8 > j) c |= 1ULL << i + j; else break;
		for(int j=1; j<8; j++)
			if(i - j * 8 > 7) c |= 1ULL << i - j * 8; else break;
		for(int j=1; j<8; j++)
			if(i + j * 8 < 56) c |= 1ULL << i + j * 8; else break;
		RelevantRookMask[i] = c;
	}
}
void InitBishopAttacks(){
	int h[] = {NORTH + WEST, NORTH + EAST, SOUTH + WEST, SOUTH + EAST};
	for(int square=0; square<64; square++){
		u64 c = 0ULL;
		int TopSpace = 7 - square/8, BottomSpace = square/8, LeftSpace = square%8, RightSpace = 7 - square%8;
		for(int j=1; j<8; j++)
			if(j < TopSpace && j < LeftSpace) c |= 1ULL << (square + j * (NORTH + WEST)); else break;
		for(int j=1; j<8; j++)
			if(j < TopSpace && j < RightSpace) c |= 1ULL << (square + j * (NORTH + EAST)); else break;
		for(int j=1; j<8; j++)
			if(j < BottomSpace && j < RightSpace) c |= 1ULL << (square + j * (SOUTH + EAST)); else break;
		for(int j=1; j<8; j++)
			if(j < BottomSpace && j < LeftSpace) c |= 1ULL << (square + j * (SOUTH + WEST)); else break;
		RelevantBishopMask[square] = c;
	}
}



void FindMagics(u64* mask, u64* magictable, u64 lookup[][4096], int piece){
	for(int square = 0; square < 64; square++){
		u64 km[4096], maskcom[4096], shiftid = BitCount(mask[square]), magic;
		u8 checkbit ;
		//fill maskcom
		
		for(int permid=0; permid < pow(2, shiftid); permid++){
			u64 c = permid, l = 0ULL, m = mask[square]; // c to l based m
			u64 t = BitCount(c);
			for(int h =0; h < t; h++){ // make l a perm of relevant mask
				const int ZeroCounter = __builtin_ctzll(c);
				for(int k=0; k< ZeroCounter; k++) m &= m - 1;
				c = c >> ZeroCounter + 1;
				l |= m & -m;
				m &= m - 1;
			}
			maskcom[permid] = l;
		}
		do{
			u64 l;
			do{magic = rand64() & rand64() & rand64();}while(!magic || BitCount(magic) >= 10);
			checkbit = 0;
			memset(km, 0, sizeof(km));

			for(int permid=0; permid < shiftid; permid++){
				l = maskcom[1ULL << permid];
				if (km[(l * magic) >> (64 - shiftid)] == 0){
					km[(l * magic) >> (64 - shiftid)] = 1ULL;
				}
				else{
					checkbit = 1;
					break;
				}
			}
			memset(km, 0, sizeof(km));
			if(checkbit == 0){
				for(int permid=0; permid< pow(2,shiftid); permid++){
					l = maskcom[permid];
					if (km[(l * magic) >> (64 - shiftid)] == 0){
						km[(l * magic) >> (64 - shiftid)] = 1ULL;
					}
					else{
						checkbit = 1;
						break;
					}
				}
			}
		}while(checkbit);
		//magic found
		magictable[square] = magic;
		for(int permid=0; permid < pow(2, shiftid); permid++){
			u64 filteredblocker = 0ULL;
			u64 l = maskcom[permid];
			if(piece == ROOK){
				//left
				for(int i=1; !(BitCheck(l, square + (i-1) * WEST)) && i <= square%8; i++)filteredblocker |= 1ULL << (square + i * WEST);
/*				for(int i=1; i<= square%8; i++){
					filteredblocker |= 1ULL << square - i;
					if(BitCheck(l, square - i) == 1)
						break;
				}
*/				//right
				for(int i=1; i<=7 - square%8; i++){
					filteredblocker |= 1ULL << square + i;
					if(BitCheck(l, square + i) == 1)
						break;
				}
				//up
				for(int i=1; i<=7 - square/8; i++){
					filteredblocker |= 1ULL << square + i * 8;
					if(BitCheck(l, square + i * 8) == 1)
						break;
				}
				//down
				for(int i=1; i<= square/8; i++){
					filteredblocker |= 1ULL << square - i * 8;
					if(BitCheck(l, square - i * 8) == 1)
						break;
				}
			}
			else if(piece == BISHOP){
				int TopSpace = 7 - square/8, BottomSpace = square/8, LeftSpace = square%8, RightSpace = 7 - square%8;
				for(int j=1; j <= TopSpace && j <= LeftSpace; j++){
					filteredblocker |= 1ULL << (square + j * (NORTH + WEST)); if(BitCheck(l, square + j * (NORTH + WEST))) break;}
				for(int j=1; j <= TopSpace && j <= RightSpace; j++){
					filteredblocker |= 1ULL << (square + j * (NORTH + EAST)); if(BitCheck(l, square + j * (NORTH + EAST))) break;}
				for(int j=1; j <= BottomSpace && j <= RightSpace; j++){
					filteredblocker |= 1ULL << (square + j * (SOUTH + EAST)); if(BitCheck(l, square + j * (SOUTH + EAST))) break;}
				for(int j=1; j <= BottomSpace && j <= LeftSpace; j++){
					filteredblocker |= 1ULL << (square + j * (SOUTH + WEST)); if(BitCheck(l, square + j * (SOUTH + WEST))) break;}
			}
			lookup[square][ (l * magic) >> (64 - shiftid)] = filteredblocker;
		}
	}
}
inline u64 GetRookWay(u64 b, int square){
	return RookBase[square][ (b * RookMagics[square]) >> (64 - BitCount(RelevantRookMask[square]))];
}
inline u64 GetBishopWay(u64 b, int square){
	return BishopBase[square][ (b * BishopMagics[square]) >> (64 - BitCount(RelevantBishopMask[square]))];
}


void Init_pestos(){
	for(int sq=0; sq < 64; sq++){ // sq = square
		for(int p = 0; p < 6; p++){ // p = piece
			mg_table[p][sq] = mg_value[p] + mg_pesto_table[p][sq];
			eg_table[p][sq] = eg_value[p] + eg_pesto_table[p][sq];
			mg_table[p + 6][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
			eg_table[p + 6][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
		}
	}
}
