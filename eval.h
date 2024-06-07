
int Evaluate(position *b){
	int mg[2] = {}; // black 0 white 1
	int eg[2] = {};
	u64 woccupied = b->occupied[WHITE];
	u64 boccupied = b->occupied[BLACK];

	int GamePhase = 0;
	
	while(woccupied){
		int i, lsi = lsb(woccupied);
		for(i=0; i < 6; i++) if(get_bit(b->bitboards[i],lsi))break;
		woccupied &= woccupied - 1;
		mg[WHITE] += mg_table[i][lsi];
		eg[WHITE] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
	}		
	while(boccupied){
		int i, lsi = lsb(boccupied);
		for(i=0; i < 6; i++) if(get_bit(b->bitboards[6 + i],lsi))break;
		boccupied &= boccupied - 1;
		mg[BLACK] += mg_table[i][lsi];
		eg[BLACK] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
	}	

	int mgScore = mg[WHITE] - mg[BLACK];
	int egScore = eg[WHITE] - eg[BLACK];
	int mgPhase = GamePhase;
	if(mgPhase > 24) mgPhase = 24; // promotion
	int egPhase = 24 - mgPhase;
	//std::cout << mgScore << " " << mgPhase << " " << egScore << " " << egPhase << std::endl;
	//std::cout << "val " <<  (mgScore * mgPhase + egScore * egPhase) / 24 << std::endl;
	return (mgScore * mgPhase + egScore * egPhase) / 24;
}
