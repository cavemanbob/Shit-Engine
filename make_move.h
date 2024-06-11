
void make_move(position *b, move _move){
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 ct_side_buffer = (_move.side == WHITE) ? 6 : 0;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	const u8 side = _move.side;
	const u64 occupied = b->occupied[WHITE] | b->occupied[BLACK];
	const u64 enemy_occupied = b->occupied[1 - side];
	const u64 enpass_buffer = (side == WHITE) ? -8 : 8;
	//const u8 captured_piece = b->captured_piece;
	//const u64 swap_key = ~(1ULL << from | 1ULL << to);
	//push_position(*b);

	//push unreversable flags
	push_flag(b->captured_piece);
	push_flag(b->enpass_sq);
	push_flag(b->castling);
	push_flag(b->fifty_move);

	//castling flag
	if(moved_piece == rook){
		if     (from == 7)  b->castling &= ~(1ULL << 3);
		else if(from == 0)  b->castling &= ~(1ULL << 2);
		else if(from == 56) b->castling &= ~(1ULL << 0);
		else if(from == 63) b->castling &= ~(1ULL << 1);
	}
	else if(moved_piece == king){
		//std::cout << "HERE";
		if(from == 4){
			b->castling &= ~(0b11 << 2);
		}
		else if(from == 60){
			b->castling &= ~(0b11 << 0);
		}
	}
	//PrintBitBoard(b->castling);	
	//capture
	if(get_bit(enemy_occupied, to)){
		int i = 0;
		for(;i < 6; i++)
			if(get_bit(b->bitboards[i + ct_side_buffer], to)) break;

		pop_bit(b->bitboards[i + ct_side_buffer],to);
		pop_bit(b->occupied[1 - side],to);
		b->captured_piece = i;
	}
	else{
		b->captured_piece = NO_CAPTURE_FLAG;
	}
	//std::cout << "side " << (int)side << std::endl;
	//PrintBitBoard(b->occupied[0]);
	//move piece from -> to
	pop_bit(b->occupied[side],from);
	set_bit(b->occupied[side],to);
	
	pop_bit(b->bitboards[moved_piece + side_buffer], from);
	set_bit(b->bitboards[moved_piece + side_buffer], to);

	//promotion
	if(move_type >= 3 && move_type < 7){
		pop_bit(b->bitboards[pawn + side_buffer],to);
		set_bit(b->bitboards[side_buffer + move_type - 3],to);
	}
	//enpass
	if(move_type >= 1 && move_type < 3){
		pop_bit(b->occupied[1 - side],eps + enpass_buffer);
		pop_bit(b->bitboards[pawn + ct_side_buffer],eps + enpass_buffer);
	}
	//castle
	//PrintBitBoard(b->bitboards[rook]);
	if(move_type == short_castle_w){
		b->castling &= ~(0b11 << 2);
		pop_bit(b->bitboards[rook + side_buffer], 7);
		set_bit(b->bitboards[rook + side_buffer], 5);
		move_bit(b->occupied[side], 7, 5);
	}
	else if(move_type == long_castle_w){
		b->castling &= ~(0b11 << 2);
		pop_bit(b->bitboards[rook + side_buffer], 0);
		set_bit(b->bitboards[rook + side_buffer], 3);
		move_bit(b->occupied[side], 0, 3);
	}
	else if(move_type == short_castle_b){
		b->castling &= ~(0b11 << 0);
		pop_bit(b->bitboards[rook + side_buffer], 63);
		set_bit(b->bitboards[rook + side_buffer], 61);
		move_bit(b->occupied[side], 63, 61);
	}
	else if(move_type == long_castle_b){
		b->castling &= ~(0b11 << 0);
		pop_bit(b->bitboards[rook + side_buffer], 56);
		set_bit(b->bitboards[rook + side_buffer], 59);
		move_bit(b->occupied[side], 56, 59);
	}
	//PrintBitBoard(b->bitboards[rook]);
	//calculate new flags
	b->move_counter++;
	b->turn = 1 - b->turn;
	b->from = from;
	b->to = to;
	if(moved_piece == pawn && abs((int)from/8 - (int)to/8) > 1){b->enpass_sq = to + enpass_buffer;} else{b->enpass_sq = NO_SQUARE;}
	if(b->captured_piece == NO_SQUARE && move_type == only_move && moved_piece != pawn){
		b->fifty_move++;
	}
	else{
		b->fifty_move = 0;
	}

	//hash based new flags
	b->hash = hash_move_piece(b->hash, _move.moved_piece, _move.from, _move.to, _move.side);
	b->hash ^= Zorbist[13][b->castling];
	b->hash ^= Zorbist[14][b->enpass_sq];
	b->hash ^= Zorbist[15][b->move_counter % 64];

	push_position(b->hash);
}


void undo_move(position *b, move _move){
	pop_position();

	//hash do same thing cuz a xor b xor b = a
	b->hash = hash_move_piece(b->hash, _move.moved_piece, _move.from, _move.to, _move.side);
	b->hash ^= Zorbist[13][b->castling];
	b->hash ^= Zorbist[14][b->enpass_sq];
	b->hash ^= Zorbist[15][b->move_counter % 64];
	//recalculate old flags
	b->move_counter--;
	b->from = _move.from;
	b->to = _move.to;
	b->turn = 1 - b->turn;
	//pop old unreversable flags
	b->fifty_move = pop_flag();
	b->castling = pop_flag();
	b->enpass_sq = pop_flag();
	const u8 from = _move.from;
	const u8 to = _move.to;
	const u8 move_type = _move.move_type;
	const u8 side_buffer = (_move.side == WHITE) ? 0 : 6;
	const u8 ct_side_buffer = (_move.side == WHITE) ? 6 : 0;
	const u8 eps = b->enpass_sq;
	const u8 moved_piece = _move.moved_piece;
	const u8 captured_piece = b->captured_piece;
	const u8 side = _move.side;
	const u64 enpass_buffer = (side == WHITE) ? -8 : 8;
	//std::cout << "\ns " << (int)side << " s" << (int)b->turn << std::endl;
	//NOTE BUG -> SIDES ARE NOT THE SAME
	//castle
	if(move_type == short_castle_w){
		pop_bit(b->bitboards[rook + side_buffer], 5);
		set_bit(b->bitboards[rook + side_buffer], 7);
		move_bit(b->occupied[side], 5, 7);
	}
	else if(move_type == long_castle_w){
		pop_bit(b->bitboards[rook + side_buffer], 3);
		set_bit(b->bitboards[rook + side_buffer], 0);
		move_bit(b->occupied[side], 3, 0);
	}
	else if(move_type == short_castle_b){
		pop_bit(b->bitboards[rook + side_buffer], 61);
		set_bit(b->bitboards[rook + side_buffer], 63);
		move_bit(b->occupied[side], 61, 63);
	}
	else if(move_type == long_castle_b){
		pop_bit(b->bitboards[rook + side_buffer], 59);
		set_bit(b->bitboards[rook + side_buffer], 56);
		move_bit(b->occupied[side], 59, 56);
	}

	//enpass
	if(move_type >= 1 && move_type < 3){
		set_bit(b->occupied[1 - side],eps + enpass_buffer);
		set_bit(b->bitboards[pawn + ct_side_buffer],eps + enpass_buffer);
	}
	//promotion
	if(move_type >= 3 && move_type < 7){
		set_bit(b->bitboards[pawn + side_buffer],to);
		pop_bit(b->bitboards[side_buffer + move_type - 3],to);
	}


	//move piece to -> from
	pop_bit(b->occupied[side],to);
	set_bit(b->occupied[side],from);
	
	pop_bit(b->bitboards[moved_piece + side_buffer], to);
	set_bit(b->bitboards[moved_piece + side_buffer], from);
	//place back the capture
	if(captured_piece != NO_CAPTURE_FLAG){ // THINK !!
		set_bit(b->bitboards[captured_piece + ct_side_buffer],to);
		set_bit(b->occupied[1 - side], to);
	}
	// Last pop flags
	b->captured_piece = pop_flag();

}

