void Swap_ScoredMove(ScoredMove *a, ScoredMove *b){
	ScoredMove c = *a;
	*a = *b;
	*b = c;
}


ScoredMove Next(position b, int depth){ // WTF ADD TO THERE A ALPHABETA OR SOMETHING LIKE THAT ????????
	//printf("info depth %d\n",depth);
	//for(int i = 0; i < game_history_size; i++) std::cout << "Chash: " << game_history[i] << std::endl;
	Node_Total = 0;
	Move_Counter = b.key & 0b1111111;
	int side = b.key >> 8 & 1ULL;
	Movelist  m;
	movegen(&m, b);
	if(m.Stack_size == 0) {ScoredMove _move = {}; return _move;} //mate
	move_order(&b, &m);
	std::vector<ScoredMove> ScoredMoves;
	float val = (side == WHITE) ? INT_MIN : INT_MAX;
	float alp = 0;
	for(int i=0; i < m.Stack_size; i++){
		if(side == WHITE){
			alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			ScoredMove _move = {m.list[i], alp};
			ScoredMoves.push_back(_move);
			if(val < alp){
				val = alp;
			}
		}
		else{
			alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			//ReadableBoard(FromTo(b, m.list[i])); std::cout << "alp " << alp << std::endl;
			ScoredMove _move = {m.list[i], alp};
			ScoredMoves.push_back(_move);
			if(val > alp){
				val = alp;
			}
		}
	}
	// Sort moves and pick
	int swapped = 1;
	while(swapped){
		swapped = 0;
		for(int i = 0; i < ScoredMoves.size() - 1; i++){
			if( (side == WHITE && ScoredMoves[i].val < ScoredMoves[i+1].val) || (side == BLACK && ScoredMoves[i].val > ScoredMoves[i+1].val)){
				Swap_ScoredMove(&ScoredMoves[i], &ScoredMoves[i+1]);
				swapped = 1;
			}
		}
	}
	int max_val_size = 0;
	for(int i = 0; i < ScoredMoves.size(); i++) if(ScoredMoves[i].val == val) max_val_size++;
	//if(max_val_size == 0) assert(0);
	ScoredMove Picked_move = ScoredMoves[rand() % max_val_size];
	int Fold_id = 0;
	position sim_pos = FromTo(b, Picked_move.move);
	while(Check_Three_Fold( Hash_Position(&sim_pos))){
		if(Fold_id == ScoredMoves.size() - 1) break;
		if( !(side == WHITE && ScoredMoves[Fold_id + 1].val >= 0) || (side == BLACK && ScoredMoves[Fold_id + 1].val <= 0) ) break;
		Picked_move = ScoredMoves[Fold_id++];
		sim_pos = FromTo(b, Picked_move.move);
	}
	return Picked_move;
}
