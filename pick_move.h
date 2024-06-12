void Swap_scored_move(scored_move *a, scored_move *b){
	scored_move c = *a;
	*a = *b;
	*b = c;
}

void swap_pv(move **a, move **b){
	move *h = *a;
	*a = *b;
	*b = h;
}

scored_move Next(position b, int depth){ // WTF ADD TO THERE A ALPHABETA OR SOMETHING LIKE THAT ????????
	memset(killer_moves, 0, sizeof(move) * 256 * KILLER_SIZE);
	Node_Total = 0;
	Move_Counter = b.move_counter;
	int side = b.turn;
	moves  m;
	movegen(&m, &b);
	if(m.size == 0) {assert(0);scored_move _move = {}; return _move;} //mate
	move_order(&b, &m);
	std::vector<scored_move> scored_moves;
	int val = MIN_SCORE;
	int alp = MIN_SCORE;
	
	move *PV_list[m.size];

	//find first moves values
	
	for(int i=0; i < m.size; i++){
		PV_list[i] = (move *)calloc(64, sizeof(move));
		make_move(&b, m.moves[i]);
		//printf("\n %s", move_to_str(m.moves[i]));
		alp = -negamax(&b, depth - 1, MIN_SCORE, MAX_SCORE, PV_list[i]);
		undo_move(&b, m.moves[i]);
		scored_move _move = {m.moves[i], alp};
		scored_moves.push_back(_move);
		PV_list[i][0] = m.moves[i];
		if(val < alp){
			val = alp;
		}
	}
	// Sort moves and pick
	int swapped = 1;
	while(swapped){
		swapped = 0;
		for(int i = 0; i < scored_moves.size() - 1; i++){
			if( (scored_moves[i].val < scored_moves[i+1].val)){
				Swap_scored_move(&scored_moves[i], &scored_moves[i+1]);
				swap_pv(&PV_list[i], &PV_list[i+1]);
				swapped = 1;
			}
		}
	}
	int max_val_size = 0;
	for(int i = 0; i < scored_moves.size(); i++) if(scored_moves[i].val == val) max_val_size++;
	if(max_val_size == 0) assert(0);
	
	u8 pick_index = rand() % max_val_size;
	scored_move picked_move = scored_moves[pick_index];
	
	//
	for(int i = 0; i < 64; i++){
		g_PV[i] = PV_list[pick_index][i];
	}
	//g_PV[0] = picked_move.move;
	//clean PV_list
	for(int i = 0;i < m.size; i++){
		free(PV_list[i]);
	}

	//for(int i =0; i < scored_moves.size(); i++){
		//printf("%s %d \n", move_to_str(scored_moves[i].move), scored_moves[i].val);
	//}	
	//std::cout << "picked_move Val = " << picked_move.val << std::endl;
/*	int Fold_id = 0;
	position sim_pos = b;
	make_move(&sim_pos,picked_move.move);
	while(Check_Three_Fold( Hash_Position(&sim_pos))){
		if(Fold_id == scored_moves.size() - 1) break;
		if( !(side == WHITE && scored_moves[Fold_id + 1].val >= 0) || (side == BLACK && scored_moves[Fold_id + 1].val <= 0) ) break;
		picked_move = scored_moves[Fold_id++];
		position sim_pos = b;
		make_move(&sim_pos,picked_move.move);
	}*/

	//make_move(&b, picked_move.move);
	//Pos_Table[b.move_counter] = b;

	return picked_move;
}
