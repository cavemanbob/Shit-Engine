

int get_victim_val(position *b, u8 sq){
	if(BitCheck(b->wp | b->bp, sq)) return 1;
	if(BitCheck(b->wn | b->bn, sq)) return 2;
	if(BitCheck(b->wb | b->bb, sq)) return 2;
	if(BitCheck(b->wr | b->br, sq)) return 3;
	if(BitCheck(b->wq | b->bq, sq)) return 5;
	if(BitCheck(b->wk | b->bk, sq)) return 10;
	ReadableBoard(*b);std::cout << (int)sq << std::endl;
	assert(0);
}

void swap_move(move *a, move *b){
	move c = *a;
	*a = *b;
	*b = c;
}

void move_order(position *b, Movelist *x){
	int l = 0;
	//for(int i = 0; i < x->Stack_size; i++)std::cout << (int)x->list[i].to << std::endl;
	//_sleep(2000);
/*	for(int i = l; i < x->Stack_size; i++){ // checks
		if( is_square_attacked(*b, x->list[i].to, GetSide(b->key))){
			ReadableBoard(*b);
			ReadableBoard(FromTo(*b, x->list[i]));
			std::cout << i << " " << l << std::endl;
			_sleep(1000);
			//std::cout << (int)x->list[i].to << std::endl;
			swap_move(&(x->list[l]), &(x->list[i]));
			//std::cout << (int)x->list[i].to << std::endl;
			l++;
		}
	}*/
	const int capture_start = l;
	for(int i = l; i < x->Stack_size; i++){ // captures
		if(BitCheck(b->occupied,x->list[i].to)){
			swap_move(&(x->list[l]), &(x->list[i]));
			l++;
		}
	}
	const int capture_end = l;
	int swapped = 1;
	while(swapped){// MVV LVA
		swapped = 0;
		for(int i = capture_start; i < capture_end - 1; i++){
			const int victim1_val = get_victim_val(b, x->list[i].to);
			const int victim2_val = get_victim_val(b, x->list[i+1].to);
			if( victim1_val > victim2_val){
				swap_move(&(x->list[i]), &(x->list[i+1]));
					swapped = 1;
			}
			else if( victim1_val == victim2_val){
				const int agressor1_val = get_victim_val(b, x->list[i].from);
				const int agressor2_val = get_victim_val(b, x->list[i+1].from);
				if(agressor1_val < agressor2_val){
					swap_move(&(x->list[i]), &(x->list[i+1]));
					swapped = 1;
				}
			}
		}
	} // NOTE MAYBE ADD IS U GO TO DANGEROUS PLACE
	//std::cout << "finished" << std::endl;
}
