

int get_victim_val(position *b, u8 sq, u8 side){
	side = 1 - side; //swap side
	if(get_bit(b->bitboards[pawn + 6 * side], sq)) return 1;
	if(get_bit(b->bitboards[knight + 6 * side], sq)) return 2;
	if(get_bit(b->bitboards[bishop + 6 * side], sq)) return 2;
	if(get_bit(b->bitboards[rook + 6 * side], sq)) return 3;
	if(get_bit(b->bitboards[queen + 6 * side], sq)) return 5;
	if(get_bit(b->bitboards[king + 6 * side], sq)) return 10;
	ReadableBoard(*b);std::cout << "get_victim_val points to empty square at: " << (int)sq << std::endl; // show board and error
	assert(0);
}

void swap_move(move *a, move *b){
	move c = *a;
	*a = *b;
	*b = c;
}

void move_order(position *b, moves *x){
	int l = 0;
	const int capture_start = l;
	for(int i = l; i < x->size; i++){ // captures
		if(get_bit(b->occupied[WHITE] | b->occupied[BLACK], x->moves[i].to)){
			swap_move(&(x->moves[l]), &(x->moves[i]));
			l++;
		}
	}
	const int capture_end = l;
	int swapped = 1;
	while(swapped){// MVV LVA
		swapped = 0;
		for(int i = capture_start; i < capture_end - 1; i++){
			const int victim1_val = get_victim_val(b, x->moves[i].to, x->moves[i].side);
			const int victim2_val = get_victim_val(b, x->moves[i+1].to, x->moves[i].side);
			if( victim1_val > victim2_val){
				swap_move(&(x->moves[i]), &(x->moves[i+1]));
					swapped = 1;
			}
			else if( victim1_val == victim2_val){
				const int agressor1_val = get_victim_val(b, x->moves[i].from, x->moves[i].side);
				const int agressor2_val = get_victim_val(b, x->moves[i+1].from, x->moves[i].side);
				if(agressor1_val < agressor2_val){
					swap_move(&(x->moves[i]), &(x->moves[i+1]));
					swapped = 1;
				}
			}
		}
	} // NOTE MAYBE ADD IS U GO TO DANGEROUS PLACE
	//std::cout << "finished" << std::endl;
}
