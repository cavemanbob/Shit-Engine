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
	state = x;
	return state;
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



std::string ctos(int x){ // cordinate to string 54 -> e4
	std::string StrPieces = "abcdefgh";
	std::string r("  ");
	r[0] = StrPieces[x%8];
	r[1] = '0' + x/8 + 1;
	//std::cout << "x " << x << " " <<  r  << std::endl;
	return r;
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



u64 Hash_Position(position *x){
	u64 key = x->occupied[WHITE] ^ x->occupied[BLACK];
	for(int i = 0; i < 12; i++){
		key ^= x->bitboards[i];
	}
	return key;
}

int Check_Three_Fold(u64 new_hash){
	int k = 0;
	for(int i = 0; i < game_history_size; i++){
		if(game_history[i] == new_hash) k++;
	}
	return (k>=2) ? 1 : 0;
}



void ApplyFen(position *b, char *fen){
	*b = {};
	//game_history = {};
	for(int i = 0; i < game_history_size; i++) game_history[i] = 0;
	game_history_size = 0;

	if(strncmp(fen, "startpos", 8) == 0) fen = START_FEN;
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
