int tri_fold_rep(position *b){
	int fold_count = 0;
	for(int i = 4; i <= b->fifty_move && i <= b->history_size; i+=2){
		if(b->history[b->history_size - i - 1] == b->hash){
			fold_count++;
			if(fold_count == 2) return 1;
		}
	}
	return 0;
}
//best i - 1 but still testable
// - (1 - b->turn) equal TEST  WHITE 27 - 34 BLACK 25 - 39
// - b->turn good white terrible black
// - 1 good white bad black TEST WHITE 131 - 107  BLACK 90 - 164 + tt
// - (1 + b->turn) + tt TEST WHITE 27 - 25  BLACK 20 - 41
// - 0  bad both

// i - 1 + tt -> WHITE 34 - 36 BLACK 30 - 41
// i - 1      -> WHITE 23 - 16 BLACK 21 - 21 // WHITE 53 - 38 BLACK 41 - 49
// no tt + deleted fifty || stealmate -> WHITE 35 - 55 BLACK 25 - 69

int Quiesce(position *b, int alpha, int beta, int PLY) {
	//const u8 PLY = Global_depth - depth;
	Node_Total++;
	int stand_pat = ((b->turn == WHITE) ? 1 : -1)  * Evaluate(b);
	if( stand_pat >= beta )
		return beta;
	if( alpha < stand_pat )
		alpha = stand_pat;
	//if(b->move_counter - Move_Counter > 6)
	//if(PLY > 6)
		//return stand_pat;
	moves  m;
	movegen(&m, b);
	//if(m.size == 0) return MIN_SCORE; //checkmate or stealmate check
	move_order(b, &m);
	for(int i = 0; i < m.size; i++){ // for all captures
		if(get_bit(b->occupied[1 - m.moves[i].side], m.moves[i].to) == 0) break; //this move is not capture
		make_move(b, m.moves[i]);
		int score = -Quiesce(b, -beta, -alpha, PLY + 1);
		undo_move(b, m.moves[i]);
		if( score >= beta )
			return beta;
		if( score > alpha )
			alpha = score;
	}
	return alpha;
}


int negamax(position *b, u8 depth, int alpha, int beta){
	const u8 PLY = Global_depth - depth;
	Node_Total++;
	
	//tt things
	int base_alpha = alpha;
	tt_entry *entry = get_tt_entry(b->hash);
	if(entry->hash == b->hash && entry->depth >= depth){
		if(entry->flag == EXACT){
			return entry->val;
		}
		else if(entry->flag == LOWER_BOUND){
			alpha = max(alpha, entry->val);
		}
		else if(entry->flag == UPPER_BOUND){
			beta = min(beta, entry->val);
		}
		if(alpha >= beta){
			return entry->val;
		}
	}

	if(depth == 0){
		//if(b->captured_piece != NO_CAPTURE_FLAG){
			//return -Quiesce(b, -beta, -alpha, 0);
		//}
		return ((b->turn == WHITE) ? 1 : -1)  * Evaluate(b);
	}
	
	moves l;
	movegen(&l, b);
	move_order(b, &l);
	int val = MIN_SCORE + 1000 * PLY; // -200k

	// only fifty -> WHITE 31 39 - BLACK 23 71
	// zero thing -> WHITE 15 23 - BLACK 12 36
	// zero thing -> WHITE 54 59 - BLACK 37 112
	
	//stealmate - fifty_move - three_fold
	if(b->fifty_move >= 99 || (l.size == 0 && is_square_attacked(b, lsb(b->bitboards[king + 6 *  (1 - b->turn)]), b->turn) == 0) || tri_fold_rep(b)){
		return 0;
	}
	
	//each move
	for(int i = 0; i < l.size; i++){
		//if(l.moves[i].from == l.moves[i].to) {printf("empty move tried search \n");assert(0);} // check point
		make_move(b, l.moves[i]);
		int did_captured = b->captured_piece;
		
		val = std::max(val, -negamax(b, depth - 1, -beta, -alpha));
	
		undo_move(b, l.moves[i]);

		//alpha-beta things
		alpha = max(alpha, val);
		if (alpha >= beta){
			if(did_captured != NO_SQUARE)
				insert_killer(l.moves[i], b->move_counter);
			break;
		}

	}


	entry->val = val;
	if (val <= base_alpha){
		entry->flag = UPPER_BOUND;
	}
	else if(val >= beta)
		entry->flag = LOWER_BOUND;
	else
		entry->flag = EXACT;
	entry->depth = depth;
	entry->hash = b->hash;
	//Note: add best move id !
	
	
	return val;
}
