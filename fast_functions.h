/*inline bool BitCheck(u64 x, int s){
	return 1ULL << s & x;
}*/
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define move_bit(bitboard, source, destination)\
		  ((bitboard) ^= 1ULL << (source) | 1ULL << (destination))


inline u8 bit_count(u64 x){
	return __builtin_popcountll(x);
}

inline u8 lsb(u64 x){
		return __builtin_ctzll(x);
}

u64 rand64(){
	u64 x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return state = x;
}

int max(std::vector<int> x){
	if(x.size() == 0) return -1;
	int max = x[0];
	for(int i=1; i < x.size(); i++){
		if(max < x[i]) max = x[i];
	}
	return max;
}

int min(std::vector<int> x){
	if(x.size() == 0) return -1;
	int min = x[0];
	for(int i=1; i < x.size(); i++){
		if(min > x[i]) min = x[i];
	}
	return min;
}

inline int max(int a, int b){
	return a > b ? a : b;
}

inline int min(int a, int b){
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


const char* ctos(int x){ // cordinate to string 54 -> e4
	const char StrPieces[] = "abcdefgh";
	static char r[] = "  ";
	r[0] = StrPieces[x%8];
	r[1] = '0' + x/8 + 1;
	return r;
}

const char* move_to_str(move m){
	static char p_str[] = " rnbqkp";
	static char str[8];
	memset(str, 0, 8);
	sprintf(str + strlen(str), "%s", ctos(m.from));
	sprintf(str + strlen(str), "%s%c", ctos(m.to), Promoting_str[(m.move_type > 2 && m.move_type < 7) ? m.move_type - 2 : 0]);
	return str;
}

void PrintBitBoard(u64 b){
	std::cout << '\n';
	for(int i=7; i>=0;i--){
		for(int j=0; j<8; j++){
			std::cout << (get_bit(b, i * 8 + j) ? "1":"0");
		}
		std::cout << '\n';
	}
}


void ReadableBoard(position b){
	std::string StrPieces = "RNBQKPrnbqkp";
	for(int i=7; i>=0; i--){
		std::cout << i+1 << ' ';
		for(int j=0;j<8;j++){
			int k;
			for(k=0; k<12; k++){
				if( b.bitboards[k] & (1ULL << i*8+j)){
					std::cout << StrPieces[k] << " ";
					break;
				}
			}
			if(b.from == i * 8 + j && !get_bit((b.occupied[0] | b.occupied[1]), b.from)){
				std::cout << "x ";
				continue;
			}
			if(k==12){
				std::cout << ". ";
			}
		}
		std::cout << '\n';
	}
	std::cout << "  A B C D E F G H\n\n";
}

u64 hash_position(position *x){
	u64 h = x->turn == WHITE ? 0 : Zorbist_Black;
	u64 occupied = x->occupied[WHITE] | x->occupied[BLACK];
	
	while(occupied){
		int i = lsb(occupied);
		int j;
		for(j = 0; j < 12; j++){
			if(get_bit(x->bitboards[j], i)) break;
		}
		h ^= Zorbist[j][i];
		occupied &= occupied - 1;
	}
	h ^= Zorbist[13][x->castling];
	if(x->enpass_sq != NO_SQUARE)h ^= Zorbist[14][x->enpass_sq];

	return h;
}
inline u64 hash_move_piece(u64 old_hash, u8 piece, u8 from, u8 to, u8 side){// black moves then turn is white
	old_hash ^= Zorbist_Black;
	old_hash ^= Zorbist[piece + 6 * (1 - side)][from];
	old_hash ^= Zorbist[piece + 6 * (1 - side)][to];
	return old_hash;
}



int Check_Three_Fold(u64 new_hash){
	int k = 0;
	for(int i = 0; i < Game_History_size; i++){
		if(Game_History[i] == new_hash) k++;
	}
	return (k>=2) ? 1 : 0;
}



void ApplyFen(position *b, char *fen){
	*b = {};
	//game_history = {};
	//for(int i = 0; i < Game_History_size; i++) Game_History[i] = 0;
	Game_History_size = 0;

	if(strncmp(fen, "startpos", 8) == 0) {fen = START_FEN;Flags_History_Size = 0;}
	char pieces[] = "RNBQKPrnbqkp";
	char strfen[200];
	strcpy(strfen, fen);
	char *t = strtok(strfen, " \n");
	int space = 0, piece = 0, sq = 56;
	while(t){
		if(space == 0){
			int i = 0;
			while(t[i] != 0){
				if(t[i] == '/'){
					sq -= 17;
				}
				else if(isdigit(t[i])){
					sq += t[i] - 49;
				}
				else{
					piece = strchr(pieces, t[i]) - pieces;
					if(piece < 6){ //white
						set_bit(b->occupied[WHITE], sq);
						set_bit(b->bitboards[piece], sq);
					}
					else{ //black
						set_bit(b->occupied[BLACK], sq);
						set_bit(b->bitboards[piece], sq);
					}
				}
				i++;
				sq++;
			}
			space++;
		}
		else if(space == 1){
			if(*t == 'w') b->turn = WHITE;
			else b->turn = BLACK;
			space++;	
		}
		else if(space == 2){
			int i = 0;
			while(t[i] != 0){
				t[i] == 'K' ? b->castling |= 1ULL << 3 :
				t[i] == 'Q' ? b->castling |= 1ULL << 2 :
				t[i] == 'k' ? b->castling |= 1ULL << 1 :
				t[i] == 'q' ? b->castling |= 1ULL << 0 : 1;
				i++;
			}
			space++;
		}
		else if(space == 3){// get enpass sq
			if(*t != '-'){
				char str[] = "abcdefgh";
				int sq = 0;
				sq += strchr(str, *t) - str;
				sq += (int)(*(t + 1) - '0' - 1) * 8; // to square when u made enpass
				//std::cout << "GEPS " << sq << std::endl;
				b->enpass_sq = sq;
			}
			else{
				b->enpass_sq = NO_SQUARE;
			}
			space++;
		}
		else if(space == 4){
			b->fifty_move = std::stoi(t);
			space++;
		}
		else if(space == 5){
			b->move_counter = std::stoi(t);
			space++;	
		}
		t = strtok(NULL, " \n");
	}
	b->hash = hash_position(b);
}

//debug functions

void print_u8(u8 x){
	for(int i = 0; i < 8; i++) std::cout << (get_bit(x,i) ? "1" : "0");
	std::cout << "\n";
}

void print_position_flags(position *b){
	std::cout << "moves_counter " << (int)b->move_counter << std::endl;
	std::cout << "turn " << (int)b->turn << std::endl;
	std::cout << "from " << (int)b->from << std::endl;
	std::cout << "to " << (int)b->to << std::endl;
	std::cout << "enpass_sq " << (int)b->enpass_sq << std::endl;
	std::cout << "castling\n";
	print_u8(b->castling);
	std::cout << "fifty_move " << (int)b->fifty_move << std::endl;
	std::cout << "captured_piece " << (int)b->captured_piece << std::endl;
}

void print_position_bitboards(position *b){
	PrintBitBoard(b->occupied[0]);
	std::cout << "BLACK\n"; 
	PrintBitBoard(b->occupied[1]);
	std::cout << "WHITE\n";
	for(int i = 0; i < 12; i++){PrintBitBoard(b->bitboards[i]);std::cout << i << std::endl;}
}



int cmp_positions(position *a, position *b){
	if(a->occupied[WHITE] != b->occupied[WHITE]) return 0;
	if(a->occupied[BLACK] != b->occupied[BLACK]) return 0;

	for(int i = 0; i < 12; i++)
		if(a->bitboards[i] != b->bitboards[i]) return 0;
	if(a->enpass_sq != b->enpass_sq) return 0;
	if(a->castling != b->castling) return 0;

	return 1;


}
