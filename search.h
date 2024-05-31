

int Quiesce(position b, int alpha, int beta) {
	Node_Total++;
	//if(Node_Total > 2416112) {ReadableBoard(b);_sleep(300);}
	int stand_pat = Evaluate(b);
	if( stand_pat >= beta )
		return beta;
	if( alpha < stand_pat )
		alpha = stand_pat;

	Movelist  m;
	movegen(&m, b);
	if(m.Stack_size == 0) return (GetSide(b.key) == WHITE) ? INT_MIN : INT_MAX; //checkmate or stealmate check
	move_order(&b, &m);
	int i;
	for(i = 0; i < m.Stack_size; i++){ // get first capture
		if(BitCheck(b.occupied, m.list[i].to) == 1) break;
	}
	for(; i < m.Stack_size; i++){ // for all captures
		if(BitCheck(b.occupied, m.list[i].to) == 0) break;
		//if(test == 1){ReadableBoard(FromTo(b, m.list[i])); _sleep(150);}
		int score = -Quiesce(FromTo(b, m.list[i]), alpha, beta);
		if( score >= beta )
			return beta;
		if( score > alpha )
			alpha = score;
	}
	return alpha;
}
int alphabeta(position b, int depth, int alpha, int beta){
	Node_Total++;
	if(depth == 0){
		if(BitCheck(b.key, 31)){ // 72
			left++;
			return Quiesce(b, alpha, beta);
		}
		else{ // 1984
			right++;
			return Evaluate(b);
		}
	}
	u8 side = b.key >> 8 & 1ULL;
	if(side == WHITE){
		int val = INT_MIN;
		Movelist  m;
		movegen(&m, b);
		move_order(&b, &m);
		if(m.Stack_size == 0) return INT_MIN;
		for(int i=0; i < m.Stack_size; i++){
	//if(BitCheck(FromTo(b, m.list[i]).key,31) == 1) {ReadableBoard(b);ReadableBoard(FromTo(b, m.list[i]));std::cout << "----------------\n";_sleep(100);}
			val = std::max(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
			if (val > beta){
				break;
			}
			alpha = std::max(alpha, val);
		}
		return val;
	}
	else{
		int val = INT_MAX;
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
