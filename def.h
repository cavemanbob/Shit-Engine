#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <random>
#include <windows.h>
#include <cassert>
#define DEBUG

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

struct bitboard{
	u64 occupied;
	u64 woccupied;
	u64 boccupied;

	u64 wr;
	u64 wn;
	u64 wb;
	u64 wq;
	u64 wk;
	u64 wp;

	u64 br;
	u64 bn;
	u64 bb;
	u64 bq;
	u64 bk;
	u64 bp;

	u32 key; // 7-0 -> move counter / 8 -> turn / 12-9 -> castling / 19-13 -> fifty move
				// castling 1111 -> W_OO W_OOO B_OO B_OOO
				// 20-27 -> enpassant
				// turn white 1 black 0

#ifdef DEBUG
	u64 oldsquare;
#endif

};

enum directions : int{
	NORTH = 8,
	WEST = -1,
	SOUTH = -8,
	EAST = +1,
	RIGHT,
	LEFT,
	UP,
	DOWN
};

struct magic{
	u64 mask;
	u64 key;
};

u64 Pnk_attacks[4][64]={};
u64 p_pushes[2][64]={};
u64 RelevantRookMask[64]={};
u64 RookBase[64][4096];
u64 RookMagics[64] = {};
u64 RelevantBishopMask[64]={};
u64 BishopBase[64][4096];
u64 BishopMagics[64] = {};
enum PieceType : int{
	ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN
};

enum Piece : int{
	ROOK_W=0, KNIGHT_W, BISHOP_W, QUEEN_W, KING_W, PAWN_W,
	ROOK_B=6, KNIGHT_B, BISHOP_B, QUEEN_B, KING_B, PAWN_B,
	En_W, En_B
};
enum side : int{
	BLACK = 0, WHITE = 1
};

u64 state = 11349138731524945662ULL; //seed
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

struct output{
	std::vector<int> from;
	std::vector<int> to;
	std::vector<int> PieceType;
};

enum Val : int{
	PAWN_VAL = 100,
	ROOK_VAL = 500,
	BISHOP_VAL = 320,
	KNIGHT_VAL = 280,
	QUEEN_VAL = 980,
	KING_VAL = 20000
};

std::string bestmove("     ");

struct game{
	u64 wtime;
	u64 btime;
	u32 winc;
	u32 binc;
	u32 movestogo;
};


