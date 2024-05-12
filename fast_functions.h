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
