
void PrintBitBoard(u64 b){
	for(int i=7; i>=0;i--){
		for(int j=0; j<8; j++){
			std::cout << (BitCheck(b, i * 8 + j) ? "1":"0");
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

void ReadableBoard(bitboard b){
	std::string StrPieces = "RNBQKPrnbqkp";
	for(int i=7; i>=0; i--){
		std::cout << i+1 << ' ';
		for(int j=0;j<8;j++){
			u64* s = &b.wr;
			int k;
			for(k=0; k<12; k++){
				if( *(s+k) & (1ULL << i*8+j)){
					std::cout << StrPieces[k] << " ";
					break;
				}
			}
#ifdef DEBUG
			if(b.oldsquare == i * 8 + j){
				std::cout << "x ";
				continue;
			}
#endif
			if(k==12){
				std::cout << ". ";
			}
		}
		std::cout << '\n';
	}
	std::cout << "  A B C D E F G H\n\n";
}

void ApplyFen(bitboard *b, char *fen){
	*b = {};
	if(strncmp(fen, "startpos", 8) == 0) fen = START_FEN;
	char pieces[] = "RNBQKPrnbqkp";
	char strfen[200];
	strcpy(strfen, fen);
	char *t = strtok(strfen, " \n");
	int space = 0, piece = 0, sq = 56;
	while(t){
		if(space == 0){
			int i = 0;
			while(*(t + i) != 0){
				if(*(t + i) == '/'){
					sq -= 17;
				}
				else if(isdigit(*(t + i))){
					sq += *(t + i) - 49;
				}
				else{
					piece = strchr(pieces, *(t+i)) - pieces;
					b->occupied ^= 1ULL << sq;
					if(piece < 6){ //white
						b->woccupied ^= 1ULL << sq;
						*(&(b->wr) + piece) ^= 1ULL << sq;
					}
					else{ //black
						b->boccupied ^= 1ULL << sq;
						*(&(b->wr) + piece) ^= 1ULL << sq;
					}
				}
				i++;
				sq++;
			}
			space++;
		}
		else if(space == 1){
			if(*t == 'w') b->key ^= 1ULL << 8;
			space++;	
		}
		else if(space == 2){
			int i = 0;
			while(*(t + i) != 0){
				*(t + i) == 'K' ? b->key |= 1ULL << 12 :
				*(t + i) == 'Q' ? b->key |= 1ULL << 11 :
				*(t + i) == 'k' ? b->key |= 1ULL << 10 :
				*(t + i) == 'q' ? b->key |= 1ULL << 9 : 1;
				i++;
			}
			space++;	
		}
		else if(space == 3){
			space++;
		}
		else if(space == 4){
			space++;
		}
		else if(space == 5){
			b->key |= std::stoi(t);
			space++;	
		}
		t = strtok(NULL, " \n");
	}
}

int Perft(bitboard b, int depth){
	if(b.wk == 0 || b.bk == 0) {return 0;}
	if(depth == 0) return 1;
	output m = movegen(b);
	int MoveCount = 0;
	for(int i=0; i < m.from.size(); i++){
		//printf("%s%s: ",ctos(m.from[i]),ctos(m.to[i]));
		//std::cout << ctos(m.from[i]) << ctos(m.to[i]) << << std::endl;
		MoveCount += Perft(FromTo(b, m.from[i], m.to[i], m.PieceType[i]), depth - 1);
	}
	return MoveCount;
}

bitboard NotationMove(bitboard b, char *inp){
	char alphabet[] = "abcdefgh";
	int from = 0, to = 0;
	from += strchr(alphabet, inp[0]) - alphabet;
	from += ((int)inp[1] - '0' - 1) * 8;
	to += strchr(alphabet, inp[2]) - alphabet;
	to += ((int)inp[3] - '0' - 1) * 8;
	int piece;
	for(piece=0; piece<12; piece++)
		if(BitCheck(*(&b.wr + piece) , from)) 
			break;
	return FromTo(b,from,to,piece);
}


#define SEARCH_DEPTH_LONG_TIME 6
#define SEARCH_DEPTH_LOW_TIME 5
#define TEXT_BUFFER 3000
void Uci(){ // fix str position
	InitPawnAttacks();
	InitPawnPushes();
	InitKnightAttacks();
	InitKingAttacks();
	InitRookAttacks();
	InitBishopAttacks();
	FindMagics(RelevantBishopMask, BishopMagics, BishopBase, BISHOP);
	FindMagics(RelevantRookMask, RookMagics, RookBase, ROOK);
	rand64();
	Init_pestos();
	srand(time(NULL));

	char text[TEXT_BUFFER] = {};
	bitboard x = {};
	struct game _game = {};
	while(strcmp(text, "quit") != 0){
		FILE *fptr = fopen("f.txt", "a");
		memset(text, 0, TEXT_BUFFER);
		fgets(text, TEXT_BUFFER, stdin);
		if(*text == ' ' || *text == '\n') continue;
		fprintf(fptr, text);
		char *t = strtok(text, " \n"); // get the first word
		if(strcmp(t, "uci") == 0){
			printf("id name Shit Engine\n");
			printf("id author Efe Burak Ostundag <Cavemanbob>\n");
			printf("uciok\n");
			fflush(stdout);
		}
		else if(strcmp(t, "position") == 0){
			t = strtok(NULL, "\n"); // get all str
			ApplyFen(&x, t);
			t = strchr(t, 'm');
			if(t){ // there are "moves" str
				t += 6;
				t = strtok(t, " \n");
				while(t){
					x = NotationMove(x, t);	
					t = strtok(NULL, " \n");
				}
			}
			//t = strtok(NULL, " \n");
		}
		else if(strcmp(t, "go") == 0){
			int Did_Next = 0;
			t = strtok(NULL, " \n");
			while(t){
				if(strcmp(t, "wtime") == 0){
					t = strtok(NULL, " \n");
					_game.wtime = std::stoi(t);
				}
				else if(strcmp(t, "btime") == 0){
					t = strtok(NULL, " \n");
					_game.btime = std::stoi(t);
				}
				else if(strcmp(t, "winc") == 0){
					t = strtok(NULL, " \n");
					_game.winc = std::stoi(t);
				}
				else if(strcmp(t, "binc") == 0){
					t = strtok(NULL, " \n");
					_game.binc = std::stoi(t);
				}
				else if(strcmp(t, "movestogo") == 0){
					t = strtok(NULL, " \n");
					_game.movestogo = std::stoi(t);
				}
				else if(strcmp(t, "go") == 0){
					t = strtok(NULL, " \n");
					if(std::stoi(t) < 8){
						x = Next(x, std::stoi(t));
					}
					else{
						x = Next(x, SEARCH_DEPTH_LONG_TIME);
					}
					Did_Next = 1;
				}
				t = strtok(NULL, " \n");
			}
			if(Did_Next == 0)if(_game.wtime < 30000 || _game.btime < 30000) x = Next(x, SEARCH_DEPTH_LOW_TIME); else x = Next(x, SEARCH_DEPTH_LONG_TIME);
		}
		else if(strcmp(t, "d") == 0){
			ReadableBoard(x);
		}
		else if(strcmp(t, "stop") == 0){
			fprintf(fptr,bestmove.c_str());
			std::cout << "bestmove " << bestmove << std::endl;
		}
		else if(strcmp(t, "isready") == 0){
			std::cout << "readyok" << std::endl;
		}
		else if(strcmp(t, "ucinewgame") == 0){
			x = {};
			//x.key ^= 1ULL << 8;
		}
		else if(strcmp(t, "eval") == 0){
			t = strtok(NULL, " \n");
			if(t){
				ApplyFen(&x, t);
				std::cout << "value " << Evaluate(x) << std::endl;
			}
			else{
				std::cout << "value " << Evaluate(x) << std::endl;
			}	
		}
		else if(strcmp(t, "perft") == 0){
			t = strtok(NULL, " \n");
			output m = movegen(x);
			int MoveCount = 0;
			if(std::stoi(t) == 1){
				for(int j = 0; j < m.from.size(); j++){
					std::cout << ctos(m.from[j]) << ctos(m.to[j]) << std::endl;
				}
				std::cout << "move" << m.from.size() << std::endl;
				break;
			}	
			for(int i=0; i < m.from.size(); i++){
				int Current_Perft = Perft(FromTo(x, m.from[i], m.to[i], m.PieceType[i]), std::stoi(t) - 1);
				//printf("%s%s: %d\n",ctos(m.from[i]),ctos(m.to[i]), Current_Perft);
				std::cout << ctos(m.from[i]) << ctos(m.to[i]) << " " << Current_Perft << std::endl;
				MoveCount += Current_Perft;
			}
			std::cout << "depth " << std::stoi(t) << " total " << MoveCount << std::endl;
		}
		fclose(fptr);
	}
}
