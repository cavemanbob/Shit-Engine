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



int Pos_Val_Table[64 * 6] = {
				0,   0,   0,   0,   0,   0,   0,   0,
            78,  83,  86,  73, 102,  82,  85,  90,
             7,  29,  21,  44,  40,  31,  44,   7,
           -17,  16,  -2,  15,  14,   0,  15, -13,
           -26,   3,  10,   9,   6,   1,   0, -23,
           -22,   9,   5, -11, -10,  -2,   3, -19,
           -31,   8,  -7, -37, -36, -14,   3, -31,
             0,   0,   0,   0,   0,   0,   0,   0,

				35,  29,  33,   4,  37,  33,  56,  50,
            55,  29,  56,  67,  55,  62,  34,  60,
            19,  35,  28,  33,  45,  27,  25,  15,
             0,   5,  16,  13,  18,  -4,  -9,  -6,
           -28, -35, -16, -21, -13, -29, -46, -30,
           -42, -28, -42, -25, -25, -35, -26, -46,
           -53, -38, -31, -26, -29, -43, -44, -53,
           -30, -24, -18,   5,  -2, -18, -31, -32,

				-59, -78, -82, -76, -23,-107, -37, -50,
           -11,  20,  35, -42, -39,  31,   2, -22,
            -9,  39, -32,  41,  52, -10,  28, -14,
            25,  17,  20,  34,  26,  25,  15,  10,
            13,  10,  17,  23,  17,  16,   0,   7,
            14,  25,  24,  15,   8,  25,  20,  15,
            19,  20,  11,   6,   7,   6,  20,  16,
            -7,   2, -15, -12, -14, -15, -10, -10,

				-66, -53, -75, -75, -10, -55, -58, -70,
            -3,  -6, 100, -36,   4,  62,  -4, -14,
            10,  67,   1,  74,  73,  27,  62,  -2,
            24,  24,  45,  37,  33,  41,  25,  17,
            -1,   5,  31,  21,  22,  35,   2,   0,
           -18,  10,  13,  22,  18,  15,  11, -14,
           -23, -15,   2,   0,   2,   0, -23, -20,
           -74, -23, -26, -24, -19, -35, -22, -69,
			  
				6,   1,  -8,-104,  69,  24,  88,  26,
            14,  32,  60, -10,  20,  76,  57,  24,
            -2,  43,  32,  60,  72,  63,  43,   2,
             1, -16,  22,  17,  25,  20, -13,  -6,
           -14, -15,  -2,  -5,  -1, -10, -20, -22,
           -30,  -6, -13, -11, -16, -11, -16, -27,
           -36, -18,   0, -19, -15, -15, -21, -38,
           -39, -30, -31, -13, -31, -36, -34, -42,

				4,  54,  47, -99, -99,  60,  83, -62,
           -32,  10,  55,  56,  56,  55,  10,   3,
           -62,  12, -57,  44, -67,  28,  37, -31,
           -55,  50,  11,  -4, -19,  13,   0, -49,
           -55, -43, -52, -28, -51, -47,  -8, -50,
           -47, -42, -43, -79, -64, -32, -29, -32,
            -4,   3, -14, -50, -57, -18,  13,   4,
            17,  30,  -3, -14,   6,  -1,  40,  18
};


u64 Node_Total = 0;


#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */







