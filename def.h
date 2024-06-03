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

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef uint64_t bitboard;


struct position{

	bitboard occupied[2]; // 0 WHITE 1 BLACK
	bitboard bitboards[12]; // wr wn wb wq wk wp - br bn bb bq bk bp
	
	u8 move_counter;
	u8 enpass_sq;
	u8 castling; // sw lw sb lb   (6 -> its for did changed flag)
	u8 fifty_move;
	u8 captured_piece;
	u8 from; //old sq last moved piece
	u8 to; //new sq last moved piece
	u8 turn;
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

enum Promoting_Flag : u8{
	NOPROMATE = 0ULL, ROOK_PROMATE = 1ULL, KNIGHT_PROMATE = 2ULL, BISHOP_PROMATE = 3ULL, QUEEN_PROMATE = 4ULL
};

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

/*
enum Val : int{
	PAWN_VAL = 100,
	ROOK_VAL = 550,
	BISHOP_VAL = 320,
	KNIGHT_VAL = 280,
	QUEEN_VAL = 1150,
	KING_VAL = 20000
};*/

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

u64 game_history[512] = {};
u64 game_history_size = 0ULL;
u8 Flags_History[512] = {};
u16 Flags_History_Size = 0;
void push_flag(u8 _flag){
	if(Flags_History_Size == 511){
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

u64 left = 0;
u64 right = 0;
//u64 test = 0;

int Pos_Val_Table[64 * 6] = {
				35,  29,  33,   4,  37,  33,  56,  50, //rook
            55,  29,  56,  67,  55,  62,  34,  60,
            19,  35,  28,  33,  45,  27,  25,  15,
             0,   5,  16,  13,  18,  -4,  -9,  -6,
           -28, -35, -16, -21, -13, -29, -46, -30,
           -42, -28, -42, -25, -25, -35, -26, -46,
           -53, -38, -31, -26, -29, -43, -44, -53,
           -30, -24, -18,   5,  -2, -18, -31, -32,

				-66, -53, -75, -75, -10, -55, -58, -70, //knight
            -3,  -6, 100, -36,   4,  62,  -4, -14,
            10,  67,   1,  74,  73,  27,  62,  -2,
            24,  24,  45,  37,  33,  41,  25,  17,
            -1,   5,  31,  21,  22,  35,   2,   0,
           -18,  10,  13,  22,  18,  15,  11, -14,
           -23, -15,   2,   0,   2,   0, -23, -20,
           -74, -23, -26, -24, -19, -35, -22, -69,

				-59, -78, -82, -76, -23,-107, -37, -50, //bishop
           -11,  20,  35, -42, -39,  31,   2, -22,
            -9,  39, -32,  41,  52, -10,  28, -14,
            25,  17,  20,  34,  26,  25,  15,  10,
            13,  10,  17,  23,  17,  16,   0,   7,
            14,  25,  24,  15,   8,  25,  20,  15,
            19,  20,  11,   6,   7,   6,  20,  16,
            -7,   2, -15, -12, -14, -15, -10, -10,

				6,   1,  -8,-104,  69,  24,  88,  26, //queen
            14,  32,  60, -10,  20,  76,  57,  24,
            -2,  43,  32,  60,  72,  63,  43,   2,
             1, -16,  22,  17,  25,  20, -13,  -6,
           -14, -15,  -2,  -5,  -1, -10, -20, -22,
           -30,  -6, -13, -11, -16, -11, -16, -27,
           -36, -18,   0, -19, -15, -15, -21, -38,
           -39, -30, -31, -13, -31, -36, -34, -42,

				4,  54,  47, -99, -99,  60,  83, -62, //king
           -32,  10,  55,  56,  56,  55,  10,   3,
           -62,  12, -57,  44, -67,  28,  37, -31,
           -55,  50,  11,  -4, -19,  13,   0, -49,
           -55, -43, -52, -28, -51, -47,  -8, -50,
           -47, -42, -43, -79, -64, -32, -29, -32,
            -4,   3, -14, -50, -57, -18,  13,   4,
            17,  30,  -3, -14,   6,  -1,  40,  18,

				0,   0,   0,   0,   0,   0,   0,   0, //pawn
            78,  83,  86,  73, 102,  82,  85,  90,
             7,  29,  21,  44,  40,  31,  44,   7,
           -17,  16,  -2,  15,  14,   0,  15, -13,
           -26,   3,  10,   9,   6,   1,   0, -23,
           -22,   9,   5, -11, -10,  -2,   3, -19,
           -31,   8,  -7, -37, -36, -14,   3, -31,
             0,   0,   0,   0,   0,   0,   0,   0
};


#define FLIP(sq) ((sq)^56)

//int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
//int eg_value[6] = { 94, 281, 297, 512,  936,  0};
//						  R    N    B    Q     K  P
int mg_value[6] = { 477, 337, 365, 1025, 0, 82};
int eg_value[6] = { 94, 281, 297, 512, 936, 0};

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


