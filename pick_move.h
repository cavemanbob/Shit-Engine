void Swap_scored_move(scored_move *a, scored_move *b){
	scored_move c = *a;
	*a = *b;
	*b = c;
}


scored_move Next(position b, int depth){ // WTF ADD TO THERE A ALPHABETA OR SOMETHING LIKE THAT ????????
	Node_Total = 0;
	Move_Counter = b.move_counter;
	int side = b.turn;
	moves  m;
	movegen(&m, b);
	if(m.size == 0) {scored_move _move = {}; return _move;} //mate
	move_order(&b, &m);
	std::vector<scored_move> scored_moves;
	int val = (side == WHITE) ? INT_MIN : INT_MAX;
	int alp = 0;
	for(int i=0; i < m.size; i++){
		//std::cout << "-> " << i << std::endl;
		//if(i == 27) test = 1;
		if(side == WHITE){
			make_move(&b, m.moves[i]);
			alp = alphabeta(b, depth - 1, INT_MIN, INT_MAX);
			//undo_move(&b, m.moves[i]);
			scored_move _move = {m.moves[i], alp};
			scored_moves.push_back(_move);
			if(val < alp){
				val = alp;
			}
		}
		else{
			make_move(&b, m.moves[i]);
			alp = alphabeta(b, depth - 1, INT_MIN, INT_MAX);
			//undo_move(&b, m.moves[i]);
			//ReadableBoard(FromTo(b, m.list[i])); std::cout << "alp " << alp << std::endl;
			scored_move _move = {m.moves[i], alp};
			scored_moves.push_back(_move);
			if(val > alp){
				val = alp;
			}
		}
	}
	// Sort moves and pick
	int swapped = 1;
	while(swapped){
		swapped = 0;
		for(int i = 0; i < scored_moves.size() - 1; i++){
			if( (side == WHITE && scored_moves[i].val < scored_moves[i+1].val) || (side == BLACK && scored_moves[i].val > scored_moves[i+1].val)){
				Swap_scored_move(&scored_moves[i], &scored_moves[i+1]);
				swapped = 1;
			}
		}
	}
	int max_val_size = 0;
	for(int i = 0; i < scored_moves.size(); i++) if(scored_moves[i].val == val) max_val_size++;
	//if(max_val_size == 0) assert(0);
	scored_move picked_move = scored_moves[rand() % max_val_size];
	int Fold_id = 0;
	position sim_pos = b;
	make_move(&sim_pos,picked_move.move);
	while(Check_Three_Fold( Hash_Position(&sim_pos))){
		if(Fold_id == scored_moves.size() - 1) break;
		if( !(side == WHITE && scored_moves[Fold_id + 1].val >= 0) || (side == BLACK && scored_moves[Fold_id + 1].val <= 0) ) break;
		picked_move = scored_moves[Fold_id++];
		position sim_pos = b;
		make_move(&sim_pos,picked_move.move);
	}
	return picked_move;
}
