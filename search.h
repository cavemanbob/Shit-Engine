

int Quiesce(position b, int alpha, int beta) {
	Node_Total++;
	int stand_pat = Evaluate(&b);
	if( stand_pat >= beta )
		return beta;
	if( alpha < stand_pat )
		alpha = stand_pat;

	moves  m;
	movegen(&m, b);
	if(m.size == 0) return (b.turn == WHITE) ? INT_MIN : INT_MAX; //checkmate or stealmate check
	move_order(&b, &m);
	int i;
	for(i = 0; i < m.size; i++){ // get first capture
		if(get_bit(b.occupied[1 - m.moves[i].side], m.moves[i].to) == 1) break;
	}
	for(; i < m.size; i++){ // for all captures
		if(get_bit(b.occupied[1 - m.moves[i].side], m.moves[i].to) == 0) break; //this move is not capture
		make_move(&b, m.moves[i]);
		int score = -Quiesce(b, -alpha, -beta);
		//undo_move(&b, m.moves[i]);
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
		if(b.captured_piece){ // 72 goes there
			left++;
			return Quiesce(b, alpha, beta);
		}
		else{ // 1984 // goes there
			right++;
			return Evaluate(&b);
		}
	}
	u8 side = b.turn;
	if(side == WHITE){
		int val = INT_MIN;
		moves  m;
		movegen(&m, b);
		move_order(&b, &m);
		if(m.size == 0) return INT_MIN;
		for(int i=0; i < m.size; i++){
	//if(get_bit(FromTo(b, m.moves[i]).key,31) == 1) {ReadableBoard(b);ReadableBoard(FromTo(b, m.list[i]));std::cout << "----------------\n";_sleep(100);}
			make_move(&b, m.moves[i]);
			val = std::max(val, alphabeta(b, depth - 1, alpha, beta));
		//undo_move(&b, m.moves[i]);
			if (val > beta){
				break;
			}
			alpha = std::max(alpha, val);
		}
		return val;
	}
	else{
		int val = INT_MAX;
		moves  m;
		movegen(&m, b);
		move_order(&b, &m);
		if(m.size == 0) return INT_MAX;
		for(int i=0; i < m.size; i++){
			make_move(&b, m.moves[i]);
			val = std::min(val, alphabeta(b, depth - 1, alpha, beta));
			//undo_move(&b, m.moves[i]);
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
		if( get_bit(b.key, 33) == 0 && (is_square_attacked(b, ALLY_KING_SQUARE, side) || is_square_attacked(b, ENEMY_KING_SQUARE, SwapSide(side)) || get_bit(b.key, 31)) ){
			b.key |= 1ULL << 33;
			return alphabeta(b, reduction_limit, alpha, beta);
		}*/
