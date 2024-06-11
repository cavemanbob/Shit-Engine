
#define cmp_moves(X, Y) (X.to == Y.to && X.from == Y.from)


int get_victim_val(position *b, u8 sq, u8 side){
	if(get_bit(b->bitboards[pawn + 6 * side], sq)) return 10;
	if(get_bit(b->bitboards[knight + 6 * side], sq)) return 20;
	if(get_bit(b->bitboards[bishop + 6 * side], sq)) return 20;
	if(get_bit(b->bitboards[rook + 6 * side], sq)) return 30;
	if(get_bit(b->bitboards[queen + 6 * side], sq)) return 50;
	if(get_bit(b->bitboards[king + 6 * side], sq)) return 0;
	ReadableBoard(*b);std::cout << "get_victim_val points to empty square at: " << (int)sq
	  	<< " side " << (int)side << std::endl; // show board and error
	assert(0);
}

void swap_moves(move *a, move *b){
	move c = *a;
	*a = *b;
	*b = c;
}

int lva[8] = {2,3,3,1,0,5};
#define KILLER_SIZE 2
move killer_moves[256][KILLER_SIZE];

void insert_killer(move new_move, int depth){
	for(int i = 0; i < KILLER_SIZE - 1; i++){
		killer_moves[depth][i] = killer_moves[depth][i + 1];
	}
	killer_moves[depth][KILLER_SIZE - 1] = new_move;
}

void move_order(position *b, moves *x){
	int l = 0;
	int vals[128]; // Max possible capture
	const int capture_start = l;
	for(int i = l; i < x->size; i++){ // captures
		if(get_bit(b->occupied[WHITE] | b->occupied[BLACK], x->moves[i].to)){
			swap_moves(&(x->moves[l]), &(x->moves[i]));
			l++;
		}
	}
	const int capture_end = l;

	for(int i = l; i < capture_end; i++){
		vals[i - l] = get_victim_val(b, x->moves[i].to, x->moves[i].side) + lva[x->moves[i].moved_piece];
	}

	//insertation SORT
	int i = capture_start + 1;
	while(i < capture_end){
		move t_move = x->moves[i];
		int t_val = vals[i];
		int j = i;
		while(j > capture_start && vals[j - 1] < t_val){
			x->moves[j] = x->moves[j - 1];
			vals[j] = vals[j - 1];
			j--;
		}
		vals[j] = t_val;
		x->moves[j] = t_move;
		i++;
	}
/*	//killer moves search
	int killer_l = capture_end;
	for(i = capture_end; i < x->size; i++){
		for(int j = 0; j < KILLER_SIZE && killer_l < x->size; j++){
			if(cmp_moves(killer_moves[b->move_counter][j], x->moves[i])){
				swap_moves(&x->moves[killer_l], &x->moves[i]);
				killer_l++;
			}
		}
	}
	//printf("killer_size %d\n", killer_l - capture_end);
	//_sleep(300);

*/
/*	int swapped = 1;
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
				const int agressor1_val = get_victim_val(b, x->moves[i].from, 1 - x->moves[i].side);
				const int agressor2_val = get_victim_val(b, x->moves[i+1].from, 1 - x->moves[i].side);
				if(agressor1_val < agressor2_val){
					swap_move(&(x->moves[i]), &(x->moves[i+1]));
					swapped = 1;
				}
			}
		}
	} // NOTE MAYBE ADD IS U GO TO DANGEROUS PLACE
	
	//MVV LVA 2
	int i = capture_start + 1;
	while(i < capture_end){
		const int victim1_val = get_victim_val(b, x->moves[i].to, x->moves[i].side);
		//const int victim2_val = get_victim_val(b, x->moves[i+1].to, x->moves[i].side);
		int j = i;
		while(j > capture_start && )

	}*/


/*	//going to a attacked square exepct capture is should be bad
	for(int i = capture_end; i < x->size - 1; i++){
		for(int j = i; j < x->size - 1; j++){
			if(is_square_attacked(b, x->moves[j].to, b->turn) && is_square_attacked(b, x->moves[j+1].to, b->turn) == 0){
				swap_move(&x->moves[j], &x->moves[j+1]);
			}
		}
	}*/
}
