
int Evaluate(position *b){
	int mg[2] = {}; // black 0 white 1
	int eg[2] = {};
	
	int GamePhase = 0;
	
	while(b->woccupied){
		int i, lsi = lsb(b->woccupied);
		for(i=0; i < 6; i++) if(BitCheck(*(&b->wr + i),lsi))break;
		b->woccupied &= b->woccupied - 1;
		mg[WHITE] += mg_table[i][lsi];
		eg[WHITE] += eg_table[i][lsi];
		GamePhase += gamephaseInc[i];
	}		
	while(b->boccupied){
		int i, lsi = lsb(b->boccupied);
		for(i=0; i < 6; i++) if(BitCheck(*(&b->br + i),lsi))break;
		b->boccupied &= b->boccupied - 1;
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
