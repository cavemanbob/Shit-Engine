
int cal_material(position *b){
	int material_score = 0;
	const int piece_scores = { 5, 3, 3, 9, 0, 1, 5, 3, 3, 9, 0, 1};
	for(int i = 0; i < 12; i++){
		material_score += bit_count(b->bitboards[i]);
	}
	return material_score;
}


// material r  n  b  q  k  p
//          5  3  3  9  0  1
// 1000 ms = 1 sec
int cal_move_time(int btime, int wtime, u8 side, int move_count, int max_time, int material){ //per ms
	const int my_time = (side == WHITE) ? wtime : btime;
	const int enemy_time = (side == WHITE) ? btime : wtime;
	const int my_ratio = (my_time * 100) / max_time;
	const int enemy_ratio = (enemy_time * 100) / max_time;

	if( move_count > 6 && material > 54 && my_ratio > 50){
		return (my_time * 5) / 100;
	}
	if( move_count > 6 && material > 54 && my_ratio > 30){
		return (my_time * 3) / 100;
	}
	if( my_ratio < 30 && my_ratio > 10){
		return (my_time * 2) / 100;
	}
	if( my_ratio < 10 && my_ratio > 5){
		return (my_time * 1) / 100;
	}
	return (my_time * 1) / 200;
}
