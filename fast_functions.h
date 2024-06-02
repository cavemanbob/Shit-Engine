inline bool BitCheck(u64 x, int s){
	return 1ULL << s & x;
}

inline u8 BitCount(u64 x){
	return __builtin_popcountll(x);
}
// _builtin_ctzll(x)
inline u8 Ls1bIndex(u64 x){
		return __builtin_ffsll(x) - 1;
}

inline u8 lsb(u64 x){
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
	for(int i=7; i>=0;i--){
		for(int j=0; j<8; j++){
			std::cout << (BitCheck(b, i * 8 + j) ? "1":"0");
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

void ReadableBoard(position b){
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
			if(b.from == i * 8 + j &&  !(b.occupied & 1ULL << b.from)){
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

inline int SwapSide(u8 side){
	return (side == WHITE) ? BLACK : WHITE;
}

inline int GetSide(u64 key){
	return (key >> 8 & 1ULL);
}


u64 Hash_Position(position *x){
	u64 key = 0ULL;
	for(int i = 0; i < 15; i++){
		key ^= *(&x->occupied +i);
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
			if(*t == 'w') b->turn = WHITE;
			space++;	
		}
		else if(space == 2){
			int i = 0;
			while(*(t + i) != 0){
/*				*(t + i) == 'K' ? b->key |= 1ULL << 12 :
				*(t + i) == 'Q' ? b->key |= 1ULL << 11 :
				*(t + i) == 'k' ? b->key |= 1ULL << 10 :
				*(t + i) == 'q' ? b->key |= 1ULL << 9 : 1;*/
				i++;
			}
			space++;	
		}
		else if(space == 3){// get enpass sq
			if(*t != '-'){
				char str[] = "abcdefgh";
				int sq = 0;
				sq += strchr(str, *t) - str;
				sq += (int)(*(t + 1) - '0' - 2) * 8;
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
