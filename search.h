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
// BEST WHITE - i - b->turn   -> i - 1
// BEST BLACK - i - (1 - b->turn)  -> i - 1

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
	const int base_alpha = alpha;
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
		Best_move_id = entry->best_move;
	}
	else{
		Best_move_id = -1;
	}

	if(depth == 0){
		//if(b->captured_piece != NO_CAPTURE_FLAG){
			//return -Quiesce(b, -beta, -alpha, 0);
		//}
		return ((b->turn == WHITE) ? 1 : -1)  * Evaluate(b); // classic
		//return ((b->turn == WHITE) ? 1 : -1)  * eval_nn(b);
	}
	
	moves l;
	movegen(&l, b);
	int val = MIN_SCORE + 1000 * PLY; // -200k

	
	//stealmate - fifty_move - three_fold - mate detection
	if(b->fifty_move >= 99 || 
			(l.size == 0 && is_square_attacked(b, lsb(b->bitboards[king + 6 *  (1 - b->turn)]), b->turn) == 0) || tri_fold_rep(b)){
		return 0;
	}
	if(l.size == 0) return val;
	
	// move_order
	move_order(b, &l);
	
	if(Best_move_id != -1){
		swap_moves(&l.moves[Best_move_id], &l.moves[0]);
	}

	int new_best_move = 0;
	
	//each move
	for(int i = 0; i < l.size; i++){
		//if(l.moves[i].from == l.moves[i].to) {printf("empty move tried search \n");assert(0);} // check point
		u64 new_hash = next_hash(b, b->hash, l.moves[i]);
		make_move(b, l.moves[i]);
		if(new_hash != b->hash) assert(0);
		
		int did_captured = b->captured_piece;
		
		val = max(val, -negamax(b, depth - 1, -beta, -alpha));
	
		undo_move(b, l.moves[i]);

		//alpha-beta things
		//alpha = max(alpha, val);
		if(val > alpha){
			alpha = val;
			new_best_move = i;
		}
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
	entry->best_move = new_best_move;
	//Note: add best move id !
	
	
	return val;
}

/*

struct i_search{
	int val;
	u8 did_end;
};
typedef struct i_search i_search;

i_search iterative_search(position *b, clock_t s_time, int m_time){ // start_time, max_time
	int depth = 4;

	moves l;
	movegen(&l, b);
	if(b->fifty_move >= 99 || (l.size == 0 && is_square_attacked(b, lsb(b->bitboards[king + 6 *  (1 - b->turn)]), b->turn) == 0) || tri_fold_rep(b))
		return 0;
	if(l.size == 0) return MIN_SCORE; //add depth based 
	move_order(b, &l);

	int val1 = MIN_SCORE;
	int c_val = MIN_SCORE;
	int i1 = 0;
	int i;
	for(i = 0; i < l.size; i++){
		if( (int)(clock() - s_time) >= m_time){
			break;
		}
		make_move(b, l.moves[i]);
		c_val = -negamax(b, depth, MIN_SCORE, MAX_SCORE);
		undo_move(b, l.moves[i]);
		if(c_val > val1){
			val1 = c_val;
			i1 = i;
		}
	}

	i_search k;
	if( i != l.size){
		k.did_end = 0;
		return k;
	}
	else{
		k.val = -val1;
		k.did_end = 1;
	}


	val1 = MIN_SCORE;
	i_search c_search;
	i1 = 0;
	for(i = 0; i < l.size; i++){
		if( (int)(clock() - s_time) >= m_time){
			break;
		}
	printf("x%d\n", i);
		make_move(b, l.moves[i]);
		c_search = iterative_search(b, s_time, m_time);
		undo_move(b, l.moves[i]);
		if(c_search.did_end = 0) break;
		if(c_search.val > val1){
			val1 = c_search.val;
			i1 = i;
		}
	}
	if( i != l.size){
		return k;
	}
	else{
		printf("m %d  %d\n",i, l.size);
		k.val = -val1;
		k.did_end = 1;
		return k;
	}

}*/
