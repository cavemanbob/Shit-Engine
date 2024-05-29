

int full_depth_moves = 4;
int reduction_limit = 5;
int red_limit = 3;

float alphabeta(position b, int depth, float alpha, float beta){
	Node_Total++;
	if(depth == 0){
/*		const u8 side = (b.key >> 8 & 1ULL) ? WHITE : BLACK;
		const u8 ENEMY_KING_SQUARE = (side == WHITE) ? Ls1bIndex(b.bk) : Ls1bIndex(b.wk);
		const u8 ALLY_KING_SQUARE = (side == WHITE) ? Ls1bIndex(b.wk) : Ls1bIndex(b.bk);
		//if(Node_Total > 450 && (is_square_attacked(b, ALLY_KING_SQUARE, (side)) || is_square_attacked(b, ENEMY_KING_SQUARE, SwapSide(side)))){
			//ReadableBoard(b);
			//_sleep(4500);
		//}
		//const u8 Current_Move = b.key & 0b1111111;
		if( BitCheck(b.key, 33) == 0 && (is_square_attacked(b, ALLY_KING_SQUARE, side) || is_square_attacked(b, ENEMY_KING_SQUARE, SwapSide(side)) || BitCheck(b.key, 31)) ){
			b.key |= 1ULL << 33;
			return alphabeta(b, reduction_limit, alpha, beta);
		}*/
		return Evaluate(b);
	}
	u8 side = b.key >> 8 & 1ULL;
	if(side == WHITE){
		float val = INT_MIN;
		Movelist  m;
		movegen(&m, b);
		for(int i=0; i < m.Stack_size; i++){
			val = std::max(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
			if (val > beta){
				break;
			}
			alpha = std::max(alpha, val);
		}
		return val;
	}
	else{
		float val = INT_MAX;
		Movelist  m;
		movegen(&m, b);
		for(int i=0; i < m.Stack_size; i++){
			val = std::min(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
			//ReadableBoard(FromTo(b, m.list[i]));
			//std::cout << "val " << val << "\n\n";
			if (val < alpha){
				break;
			}
			beta = std::min(beta, val);
		}
		return val;
	}

}

ScoredMove Next(position b, int depth){
	//printf("info depth %d\n",depth);
	Node_Total = 0;
	Move_Counter = b.key & 0b1111111;
	int side = b.key >> 8 & 1ULL;
	Movelist  m;
	movegen(&m, b);
	std::vector<ScoredMove> ScoredMove_table;
	float val = (side == WHITE) ? INT_MIN : INT_MAX;
	float alp = 0;
	for(int i=0; i < m.Stack_size; i++){
		if(side == WHITE){
			alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			ScoredMove _move = {m.list[i], alp};
			ScoredMove_table.push_back(_move);
			if(val < alp){
				val = alp;
			}
		}
		else{
			alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			//ReadableBoard(FromTo(b, m.list[i])); std::cout << "alp " << alp << std::endl;
			ScoredMove _move = {m.list[i], alp};
			ScoredMove_table.push_back(_move);
			if(val > alp){
				val = alp;
			}
		}
	}
	//std::cout << "val: " << val << std::endl;
	//for(int i=0; i < ScoredMove_table.size(); i++){ReadableBoard(FromTo(b, m.list[i]));std::cout << ScoredMove_table[i].val << std::endl;}
	// Pick random max or min value between them
	std::vector<ScoredMove> Max_Moves;
	for(int i = 0; i < ScoredMove_table.size(); i++){
		if(ScoredMove_table[i].val == val) Max_Moves.push_back(ScoredMove_table[i]);
	}
	ScoredMove Picked_move = Max_Moves[rand() % Max_Moves.size()];
	return Picked_move;
	/*
	position Selected_Board = FromTo(b, Picked_move.move);
	bestmove = ctos(Picked_move.move.from).append(ctos(Picked_move.move.to));
	//infos
	std::cout << "info nodes " << Node_Total << " string " << Picked_move.val << std::endl; 
	//best move
	std::cout << "bestmove " << bestmove << ((Selected_Board.key >> 28 & 7ULL) ? Promoting_str[Selected_Board.key >> 28 & 7ULL] : ' ') << std::endl;
	return FromTo(b, Picked_move.move);*/
}


