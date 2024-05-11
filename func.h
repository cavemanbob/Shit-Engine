#include "def.h"


/*
void PrintBitBoard(u64 b);
void ReadableBoard(bitboard b);
void FenApply(bitboard* b, std::string fen);	
void InitPawnAttacks();
void InitPawnPushes();
output movegen(bitboard b, int side);
bitboard FromTo(bitboard b, int from, int to, int piece);
*/

inline bool BitCheck(u64 x, int s){
	return 1ULL << s & x;
}

inline u64 BitCount(u64 x){
	return __builtin_popcountll(x);
}
// _builtin_ctzll(x)
inline int Ls1bIndex(u64 x){
		return __builtin_ffsll(x) - 1;
}


u64 rand64(){
	u64 x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	state = x;
	return state;
}

float max(std::vector<float> x){
	if(x.size() == 0) return -1;
	float max = x[0];
	for(int i=1; i < x.size(); i++){
		if(max < x[i]) max = x[i];
	}
	return max;
}

float min(std::vector<float> x){
	if(x.size() == 0) return -1;
	float min = x[0];
	for(int i=1; i < x.size(); i++){
		if(min > x[i]) min = x[i];
	}
	return min;
}

inline float max(float a, float b){
	return a > b ? a : b;
}

inline float min(float a, float b){
	return a < b ? a : b;
}

template <directions D>
inline bool shift(u64 b){
	u64 afilemask = 0x8080808080808080, hfilemask = 0x1010101010101010, rank1mask = 0xFF;
	if(D == RIGHT){
		return b >> 1 & ~afilemask;
	}
	else if(D == LEFT){
		return b << 1 & ~hfilemask;
	}
	else if(D == UP){
		return b << 8 & ~rank1mask;
	}
	else if(D == DOWN){
		return b >> 8;
	}
}



void PrintBitBoard(u64 b){
	for(int i=7; i>=0;i--){
		for(int j=0; j<8; j++){
			std::cout << (BitCheck(b, i * 8 + j) ? "1":"0");
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

std::string ctos(int x){ // cordinate to string 54 -> e4
	std::string StrPieces = "abcdefgh";
	std::string r("  ");
	r[0] = StrPieces[x%8];
	r[1] = '0' + x/8 + 1;
	//std::cout << "x " << x << " " <<  r  << std::endl;
	return r;
}

void ReadableBoard(bitboard b){
	std::string StrPieces = "RNBQKPrnbqkp";
	for(int i=7; i>=0; i--){
		std::cout << i+1 << ' ';
		for(int j=0;j<8;j++){
			u64* s = &b.wr;
			int k;
			for(k=0; k<12; k++){
				if( *(s+k) & (1ULL << i*8+j)){
					std::cout << StrPieces[k] << " ";
					break;
				}
			}
#ifdef DEBUG
			if(b.oldsquare == i * 8 + j){
				std::cout << "x ";
				continue;
			}
#endif
			if(k==12){
				std::cout << ". ";
			}
		}
		std::cout << '\n';
	}
	std::cout << "  A B C D E F G H\n\n";
}

/*
void FenApply(bitboard* b, char *fen){
	//char *t = strtok(fen , " ");
	*b = {}; //clean board
	std::string fenstr;
	if( strcmp(fen,"startpos") == 0){
		fenstr = START_FEN;
	}
	else {
		std::string tfenstr(fen); //check
		fenstr = tfenstr;
	}
	std::cout << std::endl << fenstr << std::endl;
	std::string StrPieces = "RNBQKPrnbqkp";
	int index = 56 , space=0;
	for(char x : fenstr){
		if(x == '/'){
			index-=16;
			continue;
		}
		else if(space == 3){
			break;
		}
		else if(StrPieces.find(x) != std::string::npos && space == 0){
			*(&(b->wr) + StrPieces.find(x)) |= 1ULL << index;
			b->occupied |= (1ULL << index);
			StrPieces.find(x) > 5 ? b->boccupied |= 1ULL << index : b->woccupied |= 1ULL << index;
		}
		else if(isdigit(x) && space==0){
			index += (int)x - 49;
		}
		else if(x == ' '){
			space++;
		}
		else if(space==1){
			x == 'w' ? b->key |= 1ULL << 8 : b->key &= ~(1ULL << 8);
		}
		else if(space==2){
			x == 'K' ? b->key |= 1ULL << 12 :
			x == 'Q' ? b->key |= 1ULL << 11 :
			x == 'k' ? b->key |= 1ULL << 10 :
			x == 'q' ? b->key |= 1ULL << 9 : 1;
			}
		index++;
	}
}
*/
void ApplyFen(bitboard *b, char *fen){
	*b = {};
	if(strncmp(fen, "startpos", 8) == 0) fen = START_FEN;
	char pieces[] = "RNBQKPrnbqkp";
	char strfen[200];
	strcpy(strfen, fen);
	char *t = strtok(strfen, " \n");
	int space = 0, piece = 0, sq = 56;
	while(t){
		if(space == 0){
			int i = 0;
			while(*(t + i) != 0){
				if(*(t + i) == '/'){
					sq -= 17;
				}
				else if(isdigit(*(t + i))){
					sq += *(t + i) - 49;
				}
				else{
					piece = strchr(pieces, *(t+i)) - pieces;
					b->occupied ^= 1ULL << sq;
					if(piece < 6){ //white
						b->woccupied ^= 1ULL << sq;
						*(&(b->wr) + piece) ^= 1ULL << sq;
					}
					else{ //black
						b->boccupied ^= 1ULL << sq;
						*(&(b->wr) + piece) ^= 1ULL << sq;
					}
				}
				i++;
				sq++;
			}
			space++;
		}
		else if(space == 1){
			if(*t == 'w') b->key ^= 1ULL << 8;
			space++;	
		}
		else if(space == 2){
			int i = 0;
			while(*(t + i) != 0){
				*(t + i) == 'K' ? b->key |= 1ULL << 12 :
				*(t + i) == 'Q' ? b->key |= 1ULL << 11 :
				*(t + i) == 'k' ? b->key |= 1ULL << 10 :
				*(t + i) == 'q' ? b->key |= 1ULL << 9 : 1;
				i++;
			}
			space++;	
		}
		else if(space == 3){
			space++;
		}
		else if(space == 4){
			space++;
		}
		else if(space == 5){
			b->key |= std::stoi(t);
			space++;	
		}
		t = strtok(NULL, " \n");
	}
}


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
/*
output movegenx(bitboard b, int side){
	const int PtrP = side == WHITE ? 0 : 6;
	const u64 maskw = ~(255ULL << 16); 
	const u64 maskb = ~(255ULL << 40);
	PrintBitBoard(maskb);
	const u64 empty = ~b.occupied;
	output r;
	u64 ways;
	u64 from;
	const u64 *PawnPtr = (&b.wp + PtrP);
	const u64 *RookPtr = (&b.wr + PtrP);
	const u64 *KnightPtr = (&b.wn + PtrP);
	const u64 *BishopPtr = (&b.wb + PtrP);
	const u64 *QueenPtr = (&b.wq + PtrP);
	const u64 *KingPtr = (&b.wk + PtrP);
	//pawns
	//ways = side == WHITE ? ((*PawnPtr << 8 | ((*PawnPtr & maskw) << 16)) & empty) : ((*PawnPtr >> 8 | (*PawnPtr & maskw) >> 16) & empty);
	ways = side == WHITE ? (*PawnPtr << 8 & ~b.occupied) | ((*PawnPtr << 9 | * PawnPtr << 7) & b.boccupied)  : ((*PawnPtr >> 8 | (*PawnPtr & maskw) >> 16) & empty);
	from = *PawnPtr;
	while(ways){
		r.from.push_back(Ls1bIndex(from));
		std::cout << Ls1bIndex(from);
		r.to.push_back(Ls1bIndex(ways));
		ways &= ways - 1;
	  	from &= from - 1;
	  	r.PieceType.push_back(side == WHITE ? PAWN_W : PAWN_B);
	}
	std::cout << "\n\n\n";
	return r;
}
*/


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
	if(b.wk == 0 || b.bk == 0){return r;}
	u64 targets = side == WHITE ? b.woccupied : b.boccupied;
	if(side == WHITE){
		while(targets){
			int i = Ls1bIndex(targets), j;
			for(j=0; j<6; j++)if(BitCheck(*(&b.wr + j),i))break;
			if(j == 5){ //pawn
				u64 ways = (Pnk_attacks[0][i] & b.boccupied) | ( 1ULL << (i + 8) & ~b.occupied); //push
				if(i < 16 && BitCheck(b.occupied, i + 8) == 0 && BitCheck(b.occupied, i + 16) == 0 ) ways |= 1ULL << i + 16; //double push
				if(i + 1 == eps && i%8 != 7){ r.from.push_back(i); r.to.push_back(eps + 8); r.PieceType.push_back(En_W);} //right enpass
				if(i - 1 == eps && i%8 != 0){ r.from.push_back(i); r.to.push_back(eps + 8); r.PieceType.push_back(En_W);} //left enpass
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(PAWN_W);}
			}
			else if(j == 1){ //knight
				u64 ways = Pnk_attacks[2][i] & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(KNIGHT_W);}
			}
			else if(j == 4){ //king
				u64 ways = Pnk_attacks[3][i] & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(KING_W);}
			}
			else if(j == 0){//rook
				u64 ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(ROOK_W);}
			}
			else if(j == 2){//bishop
				u64 ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(BISHOP_W);}
			}
			else if(j == 3){//queen
				u64 ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(QUEEN_W);}
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & emptyW;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(QUEEN_W);}
			}
			targets &= targets - 1;
		}
	}
	else{
		while(targets){
			int i = Ls1bIndex(targets), j;
			for(j=0; j<6; j++)if(BitCheck(*(&b.br + j),i))break;
			if(j == 5){ //pawn
				u64 ways = (Pnk_attacks[1][i] & b.woccupied) | ( 1ULL << (i - 8) & ~b.occupied); //push
				if(i > 47 && BitCheck(b.occupied, i - 8) == 0 && BitCheck(b.occupied, i - 16) == 0 ) ways |= 1ULL << i - 16; //double push
				if(i + 1 == eps){ r.from.push_back(i); r.to.push_back(eps - 8); r.PieceType.push_back(En_B);} //right enpass
				if(i - 1 == eps){ r.from.push_back(i); r.to.push_back(eps - 8); r.PieceType.push_back(En_B);} //left enpass
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(PAWN_B);}
			}
			else if(j == 1){ //knight
				u64 ways = Pnk_attacks[2][i] & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(KNIGHT_B);}
			}
			else if(j == 4){ //king
				u64 ways = Pnk_attacks[3][i] & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(KING_B);}
			}
			else if(j == 0){//rook
				u64 ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(ROOK_B);}
			}
			else if(j == 2){//bishop
				u64 ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(BISHOP_B);}
			}
			else if(j == 3){//queen
				u64 ways = GetRookWay(b.occupied & RelevantRookMask[i], i) & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(QUEEN_B);}
				ways = GetBishopWay(b.occupied & RelevantBishopMask[i], i) & ~b.boccupied;
				while(ways){r.from.push_back(i);r.to.push_back(Ls1bIndex(ways));ways &= ways - 1;r.PieceType.push_back(QUEEN_B);}
			}
			targets &= targets - 1;
		}
	}
	//castle
	//printf("%d %d %d %d %d \n",BitCheck(b.key, 12) , BitCheck(b.occupied,5) == 0 , BitCheck(b.occupied,6) == 0 , BitCheck(b.wr,7) , BitCheck(b.wk,4));
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


int Perft(bitboard b, int depth){
	if(b.wk == 0 || b.bk == 0) {return 0;}
	if(depth == 0) return 1;
	output m = movegen(b);
	int MoveCount = 0;
	for(int i=0; i < m.from.size(); i++){
		//printf("%s%s: ",ctos(m.from[i]),ctos(m.to[i]));
		//std::cout << ctos(m.from[i]) << ctos(m.to[i]) << << std::endl;
		MoveCount += Perft(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1);
	}
	return MoveCount;
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

float Evaluate(bitboard b){
	float mg[2] = {}; // black 0 white 1
	float eg[2] = {};
	
	int GamePhase = 0;
	
	while(b.woccupied){
		int i, lsi = Ls1bIndex(b.woccupied);
		for(i=0; i < 6; i++) if(BitCheck(*(&b.wr + i),lsi))break;
		b.woccupied &= b.woccupied - 1;
		mg[WHITE] += mg_table[i][lsi];
		eg[WHITE] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
	}		
	while(b.boccupied){
		int i, lsi = Ls1bIndex(b.boccupied);
		for(i=0; i < 6; i++) if(BitCheck(*(&b.br + i),lsi))break;
		b.boccupied &= b.boccupied - 1;
		mg[BLACK] += mg_table[i][lsi];
		eg[BLACK] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
	}	

	float mgScore = mg[WHITE] - mg[BLACK];
	float egScore = eg[WHITE] - eg[BLACK];
	int mgPhase = GamePhase;
	if(mgPhase > 24) mgPhase = 24; // promotion
	int egPhase = 24 - mgPhase;
	//std::cout << mgScore << " " << mgPhase << " " << egScore << " " << egPhase << std::endl;
	//std::cout << "val " <<  (mgScore * mgPhase + egScore * egPhase) / 24 << std::endl;
	return (mgScore * mgPhase + egScore * egPhase) / 24;
}

float alphabeta(bitboard b, int depth, float alpha, float beta, int side){
	Node_Total++;
	//printf("%d\n",depth);
	if(b.wk == 0) return INT_MIN;
	if(b.bk == 0) return INT_MAX;
	if(depth == 0)
		return Evaluate(b);
	if(side == WHITE){
		float val = INT_MIN;
		output m = movegen(b);
		for(int i=0; i < m.from.size(); i++){
			val = std::max(val, alphabeta(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1, alpha, beta, BLACK));
			if (val > beta){
				break;
			}
			alpha = std::max(alpha, val);
		}
		return val;
	}
	else{
		float val = INT_MAX;
		output m = movegen(b);
		for(int i=0; i < m.from.size(); i++){
			val = std::min(val, alphabeta(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1, alpha, beta, WHITE));
			if (val < alpha){
				break;
			}
			beta = std::min(beta, val);
		}
		return val;
	}

}

bitboard Next(bitboard b, int depth){
	//for(int i =0; i < 15; i++) PrintBitBoard(*(&b.occupied + i));
	printf("info depth %d\n",depth);
	Node_Total = 0;
	int side = (1ULL << 8 & b.key) ? WHITE : BLACK;
	//std::cout << "side " << side << std::endl;
	output m = movegen(b);
	//std::cout << "possible moves count: " << m.from.size() << std::endl;
	std::vector<float> Val_table;
	float val = (side == WHITE) ? INT_MIN : INT_MAX;
	for(int i=0; i < m.from.size(); i++){
		//ReadableBoard(FromTo(b, m.from[i], m.to[i], m.PieceType[i]));
		if(side == WHITE){
			//float alp = Search(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), BLACK, depth-1);
			float alp = alphabeta(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1, INT_MIN, INT_MAX, BLACK);
			Val_table.push_back(alp);
			if(val < alp){
				val = alp;
			}
		}
		else{
			//float alp = Search(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), WHITE, depth-1);
			float alp = alphabeta(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1, INT_MIN, INT_MAX, WHITE);
			Val_table.push_back(alp);
			if(val > alp){
				val = alp;
			}
		}
	}
	for(int i=0; i < m.from.size(); i++){
		ReadableBoard(FromTo(b, m.from[i], m.to[i], m.PieceType[i]));
		printf("val %f\n",Evaluate(FromTo(b, m.from[i], m.to[i], m.PieceType[i])));
	}
	// Pick random max or min value between them
	int Max_quantity = 0;
	for(int i=0; i < Val_table.size(); i++) if(Val_table[i] == val) Max_quantity++;
	int Max_index = rand() % Max_quantity;
	for(int i=0; i < Val_table.size(); i++){
		if(Val_table[i] == val){
			Max_quantity--;
		}
		if(Max_quantity == 0){
			bestmove = ctos(m.from[i]).append(ctos(m.to[i]));
			std::cout << "info nodes " << Node_Total << " string " << val << std::endl; 
			std::cout << "bestmove " << ctos(m.from[i]) << ctos(m.to[i]) << std::endl;
			//std::cout << "info cp " << val << std::endl;
			return FromTo(b, m.from[i], m.to[i], m.PieceType[i]);
			//return ctos(m.from[i]) + ctos(m.to[i]);
		}
	}
	//std::cout << "Error No Max Picked";
	std::cout << "bestmove " << ctos(m.from[0]) << ctos(m.to[0]) << std::endl;
	return FromTo(b, m.from[0], m.to[0], m.PieceType[0]);
	//return ctos(m.from[0]) + ctos(m.to[0]);
}
/*
void GameLoop(){
	InitPawnAttacks();
	InitPawnPushes();
	InitKnightAttacks();
	InitKingAttacks();
	InitRookAttacks();
	InitBishopAttacks();
	FindMagics(RelevantBishopMask, BishopMagics, BishopBase, BISHOP);
	FindMagics(RelevantRookMask, RookMagics, RookBase, ROOK);
	rand64();

	bitboard x={};
	std::string fen = START_FEN;
	//std::string fen = "rnbqkbnr/pp3ppp/8/N1pp4/4p3/8/PPPPPPPP/1RBQKBNR w Kkq - 0 5";
	//std::string fen = "r1b1kb1r/ppp2ppp/2n2n2/3p4/2BPqp2/2N2N1P/PPP3P1/R1BQK2R w KQkq - 0 7";
	//std::string fen = "r1b1kb1r/ppp1qppp/2n2n2/3p4/2BPPp2/2N2N1P/PPP3P1/R1BQK2R w KQkq - 0 7";
	//std::string fen = "4k3/8/1p2p3/P1BQ1P2/8/8/4R3/K7 b - - 0 1";
	//std::string fen = "8/8/2p3p1/8/3P4/8/P7/8 w - - 0 1";
	FenApply(&x, fen);
	char inp[100] = {1,0};
	while(inp[0] != 'x'){
		ReadableBoard(x);
		scanf("%s", inp);
		x = PlayerMove(x, inp);
		ReadableBoard(x);
		x = Next(x, BLACK, 3);
	}
	while(inp[0] != 'x'){
		ReadableBoard(x);
		x = Next(x, WHITE, 5);
		//ReadableBoard(x);
		//x = Next(x, BLACK, 5);
		ReadableBoard(x);
		scanf("%s", inp);
		x = PlayerMove(x, inp);
	}


}*/


bitboard NotationMove(bitboard b, char *inp){
	//std::cout << "inp " << inp << std::endl;
	char alphabet[] = "abcdefgh";
	int from = 0, to = 0;
	from += strchr(alphabet, inp[0]) - alphabet;
	from += ((int)inp[1] - '0' - 1) * 8;
	//to += alphabet.find(inp[2]);
	to += strchr(alphabet, inp[2]) - alphabet;
	to += ((int)inp[3] - '0' - 1) * 8;
	int piece;
	for(piece=0; piece<12; piece++)
		if(BitCheck(*(&b.wr + piece) , from)) 
			break;
	//std::cout << "from " << from << " to " << to << " piece " << piece;
	//std::cout << "sideN " << (b.key >> 8 & 1ULL) << std::endl;
	return FromTo(b,from,to,piece);
}


#define SEARCH_DEPTH_LONG_TIME 6
#define SEARCH_DEPTH_LOW_TIME 5
void Uci(){ // fix str position
	InitPawnAttacks();
	InitPawnPushes();
	InitKnightAttacks();
	InitKingAttacks();
	InitRookAttacks();
	InitBishopAttacks();
	FindMagics(RelevantBishopMask, BishopMagics, BishopBase, BISHOP);
	FindMagics(RelevantRookMask, RookMagics, RookBase, ROOK);
	rand64();
	Init_pestos();
	srand(time(NULL));

	char text[300] = {};
	bitboard x = {};
	struct game _game = {};
	FILE *fptr = fopen("f.txt", "w");
	while(strcmp(text, "quit") != 0){
		memset(text, 0, 300);
		fgets(text, 300, stdin);
		if(*text == ' ' || *text == '\n') continue;
		fprintf(fptr, text);
		char *t = strtok(text, " \n"); // get the first word
		if(strcmp(t, "uci") == 0){
			printf("id name Shit Engine\n");
			printf("id author Efe Burak Ostundag <Cavemanbob>\n");
			printf("uciok\n");
		}
		else if(strcmp(t, "position") == 0){
			t = strtok(NULL, "\n"); // get all str
			ApplyFen(&x, t);
			t = strchr(t, 'm');
			if(t){ // there are "moves" str
				t += 6;
				t = strtok(t, " \n");
				while(t){
					x = NotationMove(x, t);	
					t = strtok(NULL, " \n");
				}
			}
			//t = strtok(NULL, " \n");
		}
		else if(strcmp(t, "go") == 0){
			int Did_Next = 0;
			t = strtok(NULL, " \n");
			while(t){
				if(strcmp(t, "wtime") == 0){
					t = strtok(NULL, " \n");
					_game.wtime = std::stoi(t);
				}
				else if(strcmp(t, "btime") == 0){
					t = strtok(NULL, " \n");
					_game.btime = std::stoi(t);
				}
				else if(strcmp(t, "winc") == 0){
					t = strtok(NULL, " \n");
					_game.winc = std::stoi(t);
				}
				else if(strcmp(t, "binc") == 0){
					t = strtok(NULL, " \n");
					_game.binc = std::stoi(t);
				}
				else if(strcmp(t, "movestogo") == 0){
					t = strtok(NULL, " \n");
					_game.movestogo = std::stoi(t);
				}
				else if(strcmp(t, "go") == 0){
					t = strtok(NULL, " \n");
					if(std::stoi(t) < 8){
						x = Next(x, std::stoi(t));
					}
					else{
						x = Next(x, SEARCH_DEPTH_LONG_TIME);
					}
					Did_Next = 1;
				}
				t = strtok(NULL, " \n");
			}
			if(Did_Next == 0)if(_game.wtime < 30000 || _game.btime < 30000) x = Next(x, SEARCH_DEPTH_LOW_TIME); else x = Next(x, SEARCH_DEPTH_LONG_TIME);
		}
		else if(strcmp(t, "d") == 0){
			ReadableBoard(x);
		}
		else if(strcmp(t, "stop") == 0){
			fprintf(fptr,bestmove.c_str());
			std::cout << "bestmove " << bestmove << std::endl;
		}
		else if(strcmp(t, "isready") == 0){
			std::cout << "readyok" << std::endl;
		}
		else if(strcmp(t, "ucinewgame") == 0){
			x = {};
			//x.key ^= 1ULL << 8;
		}
		else if(strcmp(t, "eval") == 0){
			t = strtok(NULL, " \n");
			if(t){
				ApplyFen(&x, t);
				std::cout << "value " << Evaluate(x) << std::endl;
			}
			else{
				std::cout << "value " << Evaluate(x) << std::endl;
			}	
		}
		else if(strcmp(t, "perft") == 0){
			t = strtok(NULL, " \n");
			output m = movegen(x);
			int MoveCount = 0;
			if(std::stoi(t) == 1){
				for(int j = 0; j < m.from.size(); j++){
					std::cout << ctos(m.from[j]) << ctos(m.to[j]) << std::endl;
				}
				std::cout << "move" << m.from.size() << std::endl;
				break;
			}	
			for(int i=0; i < m.from.size(); i++){
				int Current_Perft = Perft(FromTo(x, m.from[i], m.to[i], m.PieceType[i]), std::stoi(t) - 1);
				//printf("%s%s: %d\n",ctos(m.from[i]),ctos(m.to[i]), Current_Perft);
				std::cout << ctos(m.from[i]) << ctos(m.to[i]) << " " << Current_Perft << std::endl;
				MoveCount += Current_Perft;
			}
			std::cout << "depth " << std::stoi(t) << " total " << MoveCount << std::endl;
		}
	}
	fclose(fptr);
}

/*
void TestMoveGen(){
	bitboard x={};
	std::string fen = START_FEN;
	FenApply(&x,fen);
	for(int i=0; i<1; i++){
		//ReadableBoard(x);
		output m = movegen(x,WHITE);
	  	int id = rand64() % m.from.size();
		x = FromTo(x, m.from[id], m.to[id], m.PieceType[id]);
	}
}

void Test2MoveGen(int side){
	bitboard x={};
	std::string fen = "8/p1p3pp/1PP5/P6P/2p2p2/6pp/2PP1P1P/8 w - - 0 1";
	FenApply(&x,fen);
	output m = movegen(x,side);
	for(int i = 0; i < m.from.size(); i++){
		ReadableBoard(FromTo(x, m.from[i], m.to[i], m.PieceType[i]));
	}
	std::cout << m.from.size();
}*/
