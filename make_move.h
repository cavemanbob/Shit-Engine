
void make_move(position *b, move _move){
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 ct_side_buffer = (_move.side == WHITE) ? 6 : 0;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	const u8 side = b->turn;
	const u64 occupied = b->occupied[WHITE] | b->occupied[BLACK];
	const u64 enemy_occupied = b->occupied[1 - side];
	//const u8 captured_piece = b->captured_piece;
	//const u64 swap_key = ~(1ULL << from | 1ULL << to);
	
	//flags
	b->move_counter++;
	b->from = from;
	//b->captured_piece = 0;
	if(moved_piece == rook){
		push_flag(b->castling);
		if     (from == 7)  b->castling &= ~(1ULL << 3);
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
	if(get_bit(enemy_occupied, to)){
		int i = 0;
		for(;i < 6; i++)
			if(get_bit(b->bitboards[i + ct_side_buffer], to)) break;

		pop_bit(b->bitboards[i + ct_side_buffer],to);
		b->captured_piece = i + 1;
	}

	//move piece from -> to
	pop_bit(b->occupied[side],from);

	set_bit(b->occupied[side],to);
	
	pop_bit(b->bitboards[moved_piece - 1 + side_buffer], from);
	set_bit(b->bitboards[moved_piece - 1 + side_buffer], to);

	//promotion
	if(move_type >= 3 && move_type < 7){
		pop_bit(b->bitboards[pawn + side_buffer],to);
		set_bit(b->bitboards[side_buffer + move_type - 3],to);
	}
	//enpass
	if(move_type >= 1 && move_type < 3){
		pop_bit(b->occupied[1 - side],eps);
		pop_bit(b->occupied[pawn + ct_side_buffer],eps);
	}
	//castle
	if(move_type == short_castle_w){
		b->castling &= 0b11 << 2;
		pop_bit(b->occupied[rook], 7);
		set_bit(b->occupied[rook], 5);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == long_castle_w){
		b->castling &= 0b11 << 2;
		pop_bit(b->occupied[rook], 0);
		set_bit(b->occupied[rook], 3);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == short_castle_b){
		b->castling &= 0b11 << 0;
		pop_bit(b->occupied[rook], 63);
		set_bit(b->occupied[rook], 61);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == long_castle_b){
		b->castling &= 0b11 << 0;
		pop_bit(b->occupied[rook], 56);
		set_bit(b->occupied[rook], 59);
		move_bit(b->occupied[rook], from, to);
	}
}


void undo_move(position *b, move _move){
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 ct_side_buffer = (_move.side == WHITE) ? 6 : 0;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	const u8 captured_piece = b->captured_piece;
	const u8 side = _move.side;

	//flags
	b->move_counter--;
	b->from = NO_SQUARE;
	if(moved_piece == rook || moved_piece == king){
		b->castling = pop_flag();
	}

	//castle
	if(move_type == short_castle_w){
		pop_bit(b->occupied[rook], 5);
		set_bit(b->occupied[rook], 7);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == long_castle_w){
		pop_bit(b->occupied[rook], 3);
		set_bit(b->occupied[rook], 0);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == short_castle_b){
		pop_bit(b->occupied[rook], 61);
		set_bit(b->occupied[rook], 63);
		move_bit(b->occupied[rook], from, to);
	}
	else if(move_type == long_castle_b){
		pop_bit(b->occupied[rook], 59);
		set_bit(b->occupied[rook], 56);
		move_bit(b->occupied[rook], from, to);
	}

	//enpass
	if(move_type >= 1 && move_type < 3){
		set_bit(b->occupied[1 - side],eps);
		set_bit(b->occupied[pawn + ct_side_buffer],eps);
	}
	//promotion
	if(move_type >= 3 && move_type < 7){
		set_bit(b->bitboards[pawn + side_buffer],to);
		pop_bit(b->bitboards[side_buffer + move_type - 3],to);
	}

	b->move_counter--;

	//move piece from -> to
	pop_bit(b->occupied[side],to);

	set_bit(b->occupied[_move.side],from);
	
	pop_bit(b->bitboards[moved_piece - 1 + side_buffer], to);
	set_bit(b->bitboards[moved_piece - 1 + side_buffer], from);

	//place back the capture
	if(captured_piece){
		set_bit(b->bitboards[captured_piece - 1 + ct_side_buffer],to);
		set_bit(b->occupied[1 - side], to);
	}
}

