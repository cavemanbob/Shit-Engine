

int full_depth_moves = 4;
int reduction_limit = 5;
int red_limit = 3;

float alphabeta(position b, int depth, float alpha, float beta){
	Node_Total++;
	if(depth == 0){
		return Evaluate(b);
	}
	u8 side = b.key >> 8 & 1ULL;
	if(side == WHITE){
		float val = INT_MIN;
		Movelist  m;
		movegen(&m, b);
		move_order(&b, &m);
		if(m.Stack_size == 0) return INT_MIN;
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
		move_order(&b, &m);
		if(m.Stack_size == 0) return INT_MAX;
		for(int i=0; i < m.Stack_size; i++){
			val = std::min(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
			if (val < alpha){
				break;
			}
			beta = std::min(beta, val);
		}
		return val;
	}

}





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
