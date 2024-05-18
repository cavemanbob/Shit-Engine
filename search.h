


float alphabeta(position b, int depth, float alpha, float beta){
	Node_Total++;
	if(depth == 0)
		return Evaluate(b);
	u8 side = b.key >> 8 & 1ULL;
	if(side == WHITE){
		float val = INT_MIN;
		Movelist  m;
		movegen(&m, b);
		for(int i=0; i < m.Stack_size; i++){
			val = std::max(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
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
			val = std::min(val, alphabeta(FromTo(b, m.list[i]), depth - 1, alpha, beta));
			if (val < alpha){
				break;
			}
			beta = std::min(beta, val);
		}
		return val;
	}

}

position Next(position b, int depth){
	printf("info depth %d\n",depth);
	Node_Total = 0;
	int side = b.key >> 8 & 1ULL;
	Movelist  m;
	movegen(&m, b);
	std::vector<float> Val_table;
	float val = (side == WHITE) ? INT_MIN : INT_MAX;
	for(int i=0; i < m.Stack_size; i++){
		if(side == WHITE){
			float alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			Val_table.push_back(alp);
			if(val < alp){
				val = alp;
			}
		}
		else{
			float alp = alphabeta(FromTo(b, m.list[i]), depth - 1, INT_MIN, INT_MAX);
			Val_table.push_back(alp);
			if(val > alp){
				val = alp;
			}
		}
	}
	//for(int i=0; i < Val_table.size(); i++){ReadableBoard(FromTo(b, m.list[i]));std::cout << Val_table[i] << std::endl;}
	// Pick random max or min value between them
	int Max_quantity = 0;
	for(int i=0; i < Val_table.size(); i++) if(Val_table[i] == val) Max_quantity++;
	int Max_index = rand() % Max_quantity;
	for(int i=0; i < Val_table.size(); i++){
		if(Max_index == 0){
			position Selected_Board = FromTo(b, m.list[i]);
			bestmove = ctos(m.list[i].from).append(ctos(m.list[i].to));
			//infos
			std::cout << "info nodes " << Node_Total << " string " << val << std::endl; 
			//best move
			std::cout << "bestmove " << bestmove << ((Selected_Board.key >> 28 & 7ULL) ? Promoting_str[Selected_Board.key >> 28 & 7ULL] : ' ') << std::endl;
			return FromTo(b, m.list[i]);
		}
		if(Val_table[i] == val){
			Max_index--;
		}
	}
	std::cout << "bestmove " << ctos(m.list[0].from) << ctos(m.list[0].to) << std::endl;
	return FromTo(b, m.list[0]);
}
