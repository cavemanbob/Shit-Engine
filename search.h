


float alphabeta(bitboard b, int depth, float alpha, float beta, int side){
	Node_Total++;
	if(b.wk == 0) return INT_MIN;
	if(b.bk == 0) return INT_MAX;
	if(depth == 0)
		return Evaluate(b);
	if(side == WHITE){
		float val = INT_MIN;
		Movelist  m;
		movegen(&m, b);
		for(int i=0; i < m.Stack_size; i++){
			val = std::max(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta, BLACK));
			if (val > beta){
				break;
			}
			alpha = std::max(alpha, val);
		}
		return val;
	}
	else{
		float val = INT_MAX;
		Movelist  m;
		movegen(&m, b);
		for(int i=0; i < m.Stack_size; i++){
			val = std::min(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta, WHITE));
			if (val < alpha){
				break;
			}
			beta = std::min(beta, val);
		}
		return val;
	}

}

bitboard Next(bitboard b, int depth){
	printf("info depth %d\n",depth);
	Node_Total = 0;
	int side = (1ULL << 8 & b.key) ? WHITE : BLACK;
	Movelist  m;
	movegen(&m, b);
	std::vector<float> Val_table;
	float val = (side == WHITE) ? INT_MIN : INT_MAX;
	for(int i=0; i < m.Stack_size; i++){
		if(side == WHITE){
			if(OnCheck(FromTo(b, m.list[i]))){Val_table.push_back(INT_MIN);continue;}
			float alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX, BLACK);
			Val_table.push_back(alp);
			if(val < alp){
				val = alp;
			}
		}
		else{
			if(OnCheck(FromTo(b, m.list[i]))){Val_table.push_back(INT_MAX);continue;}
			float alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX, WHITE);
			Val_table.push_back(alp);
			if(val > alp){
				val = alp;
			}
		}
	}
	//for(int i=0; i < Val_table.size(); i++) std::cout << Val_table[i] << std::endl;
	// Pick random max or min value between them
	int Max_quantity = 0;
	for(int i=0; i < Val_table.size(); i++) if(Val_table[i] == val) Max_quantity++;
	int Max_index = rand() % Max_quantity;
	for(int i=0; i < Val_table.size(); i++){
		if(Val_table[i] == val){
			Max_quantity--;
		}
		if(Max_quantity == 0){
			bitboard Selected_Board = FromTo(b, m.list[i]);
			bestmove = ctos(m.list[i].from).append(ctos(m.list[i].to));
			//bestmove[4] = Promoting_str[Selected_Board.key >> 28 & 7ULL]; //promoting flag
			//infos
			std::cout << "info nodes " << Node_Total << " string " << val << std::endl; 
			//best move
			std::cout << "bestmove " << bestmove << ((Selected_Board.key >> 28 & 7ULL) ? Promoting_str[Selected_Board.key >> 28 & 7ULL] : ' ') << std::endl;
			//if(Selected_Board.key >> 28 & 7ULL) std::cout << 
			return FromTo(b, m.list[i]);
		}
	}
	//std::cout << "Error No Max Picked";
	std::cout << "bestmove " << ctos(m.list[0].from) << ctos(m.list[0].to) << std::endl;
	//free(m);
	return FromTo(b, m.list[0]);
	//return ctos(m.from[0]) + ctos(m.to[0]);
}
