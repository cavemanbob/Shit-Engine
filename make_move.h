
u8 make_move(position *b, move _move){
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 ct_side_buffer = (_move.side == WHITE) ? 6 : 0;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	//const u8 captured_piece = b->captured_piece;
	//const u64 swap_key = ~(1ULL << from | 1ULL << to);
	
	//flags
	b->move_counter++;
	b->from = from;
	if(moved_piece == rook){
		push_flag(b->castling);
		if(from == 7)       b->castling &= ~(1ULL << 3);
		else if(from == 0)  b->castling &= ~(1ULL << 2);
		else if(from == 56) b->castling &= ~(1ULL << 1);
		else if(from == 53) b->castling &= ~(1ULL << 0);
	}
	else if(moved_piece == king){
		push_flag(b->castling);
		if(from == 4){
			b->castling &= ~(0b11 << 2);
		}
		else if(from == 60){
			b->castling &= ~(0b11 << 0);
		}
	}
	
	//capture
	if(b->occupied & 1ULL << to){
		int i = 0;
		for(;i < 6; i++)
			if(*(&b->wr + i + ct_side_buffer) & 1ULL << to) break;

		*(&b->wr + i + ct_side_buffer) &= ~(1ULL << to);
		b->captured_piece = i + 1;
	}

	//move piece from -> to
	b->occupied  &= ~(1ULL << from);
	b->woccupied &= ~(1ULL << from);
	b->boccupied &= ~(1ULL << from);

	b->occupied  |= 1ULL << to;
	if(_move.side == WHITE) 
		b->woccupied |= 1ULL << to; 
	else 
		b->boccupied |= 1ULL << to;
	
	*(&b->wr + moved_piece - 1 + side_buffer) ^= 1ULL << from | 1ULL << to;

	//promotion
	if(move_type >= 3){
		*(&b->wp + side_buffer) &= ~(1ULL << to);
		*(&b->wr + side_buffer + move_type - 3) |= 1ULL << to;
	}
	else{ //no-pro_move 
		*(&b->wr + side_buffer + moved_piece - 1) |= 1ULL << to;
	}
	//enpass
	if(move_type >= 1 && move_type <= 6){
		b->occupied  &= ~(1ULL << eps);
		b->woccupied &= ~(1ULL << eps);
		b->boccupied &= ~(1ULL << eps);
		*(&b->wp + 6 - side_buffer) &= ~(1ULL << eps);
	}
	//castle
	if(move_type == short_castle_w){b->wr &= 1ULL << 7; b->wr |= 1ULL << 5; b->castling &= 0b11 << 2;}
	else if(move_type == long_castle_w){b->wr &= 1ULL << 0; b->wr |= 1ULL << 3; b->castling &= 0b11 << 2;}
	else if(move_type == short_castle_b){b->br &= 1ULL << 63; b->wr |= 1ULL << 61; b->castling &= 0b11 << 0;}
	else if(move_type == long_castle_b){b->br &= 1ULL << 56; b->wr |= 1ULL << 59; b->castling &= 0b11 << 0;}
	return b->castling;
}


void undo_move(position *b, move _move){
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	const u8 captured_piece = b->captured_piece;

	//flags
	b->move_counter--;
	b->from = NO_SQUARE;
	if(moved_piece == rook || moved_piece == king){
		b->castling = pop_flag();
	}

	//castle
	if(move_type == short_castle_w){b->wr |= 1ULL << 7; b->wr &= 1ULL << 5;}
	else if(move_type == long_castle_w){b->wr |= 1ULL << 0; b->wr &= 1ULL << 3;}
	else if(move_type == short_castle_b){b->br |= 1ULL << 63; b->wr &= 1ULL << 61;}
	else if(move_type == long_castle_b){b->br |= 1ULL << 56; b->wr &= 1ULL << 59;}

	//enpass
	if(move_type >= 1 && move_type <= 6){
		b->occupied  |= 1ULL << eps;
		b->woccupied |= 1ULL << eps;
		b->boccupied |= 1ULL << eps;
		*(&b->wp + 6 - side_buffer) |= 1ULL << eps;
	}
	//promotion
	if(move_type >= 3){
		*(&b->wp + side_buffer) |= 1ULL << to;
		*(&b->wr + side_buffer + move_type - 3) &= ~(1ULL << to);
	}
	else{ //no-pro_move 
		*(&b->wr + side_buffer + moved_piece - 1) &= ~(1ULL << to);
	}

	b->move_counter--;

	//move piece from -> to
	b->occupied  &= ~(1ULL << to);
	b->woccupied &= ~(1ULL << to);
	b->boccupied &= ~(1ULL << to);

	b->occupied  |= 1ULL << from;
	if(_move.side == WHITE) 
		b->woccupied |= 1ULL << from; 
	else 
		b->boccupied |= 1ULL << from;
	
	*(&b->wr + moved_piece - 1 + side_buffer) ^= 1ULL << from | 1ULL << to;

	//place back the capture
	if(captured_piece){
		*(&b->wr + (captured_piece - 1 + side_buffer)) |= 1ULL << to;
		b->occupied  |= 1ULL << to;
		*(&b->woccupied + b->turn) |= 1ULL << to;
	}
}

