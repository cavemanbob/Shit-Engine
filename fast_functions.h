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
