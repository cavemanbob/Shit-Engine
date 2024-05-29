
void swap_move(move *a, move *b){
	move c = *a;
	*a = *b;
	*b = c;
}

void move_order(position *b, Movelist *x){
	int l = 0;
	for(int i = l; i < x->Stack_size; i++){ // checks
		if( is_square_attacked(*b, x->list[i].to, SwapSide(GetSide(b->key))))
			swap_move(&(x->list[l]), &(x->list[i]));
	}
	for(int i = l; i < x->Stack_size; i++){ // captures
		if(x->list[i].from != x->list[i].to && BitCheck(b->occupied,x->list[i].to))
			swap_move(&(x->list[l]), &(x->list[i]));
	}
}
