#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <random>
//#include <windows.h>
#include <cassert>
#define DEBUG
#define MIN_SCORE (-200000)
#define MAX_SCORE (200000)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef uint64_t bitboard;


struct position{

	bitboard occupied[2]; // 0 WHITE 1 BLACK
	bitboard bitboards[12]; // wr wn wb wq wk wp - br bn bb bq bk bp

	//recalculatable flags	
	u8 move_counter;
	u8 turn;
	u8 from; //old sq last moved piece
	u8 to; //new sq last moved piece
	
	//not_reversable flags
	u8 enpass_sq;
	u8 castling; // sw lw sb lb   (6 -> its for did changed flag)
	u8 fifty_move;
	u8 captured_piece;
	
	u64 hash;
};

#define NO_SQUARE 65

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

u64 Pnk_attacks[4][64]={}; // PawnW PawnB Knight King
u64 p_pushes[2][64]={};
u64 RelevantRookMask[64]={};
u64 RookBase[64][4096];
u64 RookMagics[64] = {};
u64 RelevantBishopMask[64]={};
u64 BishopBase[64][4096];
u64 BishopMagics[64] = {};
u64 FullRelevantRookMask[64]={};// added corners
u64 FullRelevantBishopMask[64]={};// added corners
enum PieceType : u64{
	ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN
};

enum Piece : u8{
	ROOK_W=0, KNIGHT_W = 1, BISHOP_W = 2, QUEEN_W = 3, KING_W = 4, PAWN_W = 5,
	ROOK_B=6, KNIGHT_B = 7, BISHOP_B = 8, QUEEN_B = 9, KING_B = 10, PAWN_B = 11,
	En_W = 12, En_B = 13, Pro_R = 14, Pro_N = 15, Pro_B = 16, Pro_Q = 17
};

const char Promoting_str[8] = " rnbq";


enum side : int{
	BLACK = 0, WHITE = 1
};
enum FileMask : u64{
	HFILEMASK = 0x8080808080808080,
	AFILEMASK = 0x0101010101010101,
	RANK8MASK = 0xFF00000000000000,
	RANK1MASK = 0x00000000000000FF
};


u64 state = 11349138731524945662ULL; //seed
//#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
char START_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


std::string bestmove = "      ";

struct game{
	u64 wtime;
	u64 btime;
	u32 winc;
	u32 binc;
	u32 movestogo;
};
u64 Node_Total = 0;
u8 Move_Counter = 0; // move for current fen
u8 Global_depth = 0;


enum piece_types_enum : u8{
	rook = 0, knight = 1, bishop = 2, queen = 3, king = 4, pawn = 5
};

struct move{
	u8 from;
	u8 to;
	u8 moved_piece;
	//u8 captured_piece;
	//this is should be in position
	u8 move_type;
	u8 side;
};
enum move_types : u8{
	only_move = 0, en_w = 1, en_b = 2, pro_r = 3, pro_n = 4, pro_b = 5, pro_q = 6,
  	short_castle_w, long_castle_w, short_castle_b, long_castle_b
};
#define NO_CAPTURE_FLAG 16

struct scored_move{
	move move;
	int val;
};
typedef struct scored_move scored_move;

struct moves{
	move moves[256];
	int size;
};

inline void moves_add(moves *source, move move){
	source->moves[source->size++] = move;
}

u64 Game_History[512] = {};
u64 Game_History_size = 0ULL;
void push_position(u64 hash){
	if(Game_History_size == 511){
		printf("\nposition_stack capacity is full, push is not possible!\n");
		assert(0);
	}
	Game_History[Game_History_size++] = hash;
}
u64 pop_position(){
	if(Game_History_size == 0){
		printf("\nposition_stack Size is 0, pop is not possible!\n");
		assert(0);
	}
	return Game_History[--Game_History_size];
}

u8 Flags_History[512 * 4] = {};
u16 Flags_History_Size = 0;
void push_flag(u8 _flag){
	if(Flags_History_Size == 512 * 4 - 1){
		printf("\nFlag capacity is full, push is not possible!\n");
		assert(0);
	}
	Flags_History[Flags_History_Size++] = _flag;
}
u8 pop_flag(){
	if(Flags_History_Size == 0){
		printf("\nFlag Size is 0, pop is not possible!\n");
		assert(0);
	}
	return Flags_History[--Flags_History_Size];
}


u64 Hash_flag_history[512] = {};
u16 Hash_flag_history_size = 0;
void push_hash_flag(u64 hash){
	if(Hash_flag_history_size == 511){
		printf("\nHash flag capacity is full, push is not possible!\n");
		assert(0);
	}
	Hash_flag_history[Hash_flag_history_size++] = hash;
}
u64 pop_hash_flag(){
	if(Hash_flag_history_size == 0){
		printf("\nHash flag size is 0, pop is not possible!\n");
		assert(0);
	}
	return Hash_flag_history[--Hash_flag_history_size];
}



u64 Zorbist[18][64];
u64 Zorbist_Black;


position Pos_Table[512] = {};
//u64 PV[64][64] = {}; // possible move count in a position - depth
move g_PV[64] = {};




#define FLIP(sq) ((sq)^56)

//int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
//int eg_value[6] = { 94, 281, 297, 512,  936,  0};
//						  R    N    B    Q     K  P
int mg_value[6] = { 477, 337, 365, 1025, 0, 82};
int eg_value[6] = { 512, 281, 297, 936, 0, 94};

int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

int* mg_pesto_table[6] =
{
    mg_rook_table,
    mg_knight_table,
    mg_bishop_table,
    mg_queen_table,
    mg_king_table,
    mg_pawn_table
};

int* eg_pesto_table[6] =
{
    eg_rook_table,
    eg_knight_table,
    eg_bishop_table,
    eg_queen_table,
    eg_king_table,
    eg_pawn_table
};


//int gamephaseInc[12] = {0,0,1,1,1,1,2,2,4,4,0,0};
int gamephaseInc[6] = {2,1,1,4,0,0};
int mg_table[12][64];
int eg_table[12][64];

enum Squares : u8{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8 = 63ULL
};



