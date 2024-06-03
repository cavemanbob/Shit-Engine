

u64 perft(position b, int depth){
/*	if(depth == 1){
		moves  m;
		movegen(&m, b);
		return m.size;
	}*/
	if(depth == 0) return 1;
	moves  m;
	movegen(&m, b);
	u64 move_count = 0;
	for(int i=0; i < m.size; i++){
					//if(m.moves[i].to == 16) ReadableBoard(b);
		make_move(&b, m.moves[i]);
		move_count += perft(b, depth - 1);
		undo_move(&b, m.moves[i]);
	}
	return move_count;
}

position notation_move(position b, char *inp){
	const u64 occupied = b.occupied[WHITE] | b.occupied[BLACK];
	char alphabet[] = "abcdefgh";
	u8 from = 0, to = 0;
	from += strchr(alphabet, inp[0]) - alphabet;
	from += ((u8)inp[1] - '0' - 1) * 8;
	to += strchr(alphabet, inp[2]) - alphabet;
	to += ((u8)inp[3] - '0' - 1) * 8;
	if(inp[4] != 0 && inp[4] != ' '){//promating move
		move _move = {from, to, pawn, (u8)((strchr(Promoting_str, inp[4]) - Promoting_str) + 2)};
		make_move(&b, _move);
		return b;
	}
	int piece;
	for(piece=0; piece<12; piece++)
		if(get_bit(b.bitboards[piece] , from)) 
			break;
	if( (piece == PAWN_W || piece == PAWN_B) && (abs((int)from % 8 - (int)to % 8) > 0) && get_bit(occupied, to) == 0){
		move _move = {from, to, pawn, b.turn == WHITE ? en_w : en_b};
		make_move(&b, _move);
		return b;
	}
	move _move = {from, to, (u8)(piece + 1 - 6 * (1 - b.turn)), only_move};
	make_move(&b, _move);
	return b;
}



#define SEARCH_DEPTH_LONG_TIME 6
#define SEARCH_DEPTH_LOW_TIME 5
#define TEXT_BUFFER 3000
void Uci(){
	init_all();

	char text[TEXT_BUFFER] = {};
	position x = {};
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
			//printf("option name Threads type spin default 1 min 1 max 1");
			printf("uciok\n");
			fflush(stdout);
		}
		else if(strcmp(t, "position") == 0){
			t = strtok(NULL, "\n"); // get all str
			ApplyFen(&x, t);
			game_history[game_history_size++] = Hash_Position(&x);
			t = strchr(t, 'm');
			if(t){ // there are "moves" str
				t += 6;
				t = strtok(t, " \n");
				while(t){
					x = notation_move(x, t);	
					game_history[game_history_size++] = Hash_Position(&x);
					t = strtok(NULL, " \n");
				}
			}
			//t = strtok(NULL, " \n");
		}
		else if(strcmp(t, "go") == 0){
			int _depth = 1;
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
				else if(strcmp(t, "movetime") == 0){
					t = strtok(NULL, " \n");
					if(std::stoi(t) < 3000){
						_depth = SEARCH_DEPTH_LOW_TIME;
					}
					else{
						_depth = SEARCH_DEPTH_LONG_TIME;
					}

				}
				else if(isdigit(*t) == 1){
					_depth = std::stoi(t);
				}
			//if(Did_Next == 0)if(_game.wtime < 30000 || _game.btime < 30000) x = Next(x, SEARCH_DEPTH_LOW_TIME); else x = Next(x, SEARCH_DEPTH_LONG_TIME);
				t = strtok(NULL, " \n");
			}
			//time control
			if(_depth == 1 && (_game.wtime < 30000 || _game.btime < 30000)){
				_depth = SEARCH_DEPTH_LOW_TIME;
			}
			else if(_depth == 1){
				_depth = SEARCH_DEPTH_LOW_TIME;
			}
			Global_depth = _depth;
			//infos
			clock_t ct = clock();
			std::cout << "info depth " << _depth << std::endl;
			scored_move Picked_move = Next(x, _depth);
			//x = FromTo(x, Picked_move.move);
			make_move(&x, Picked_move.move);
			position _x = x;
			bestmove = ctos(Picked_move.move.from).append(ctos(Picked_move.move.to));
			std::cout << "info nodes " << Node_Total << std::endl;
			std::cout << "bestmove " << bestmove << ((Picked_move.move.move_type > 1 && 7 > Picked_move.move.move_type) ? Promoting_str[Picked_move.move.move_type - 2] : ' ') << std::endl;
			std::cout << "info pv ";
	/*		for(_depth = _depth - 1;_depth > 0; _depth--){
				//std::cout << "depth" << _depth;
				Picked_move = Next(x, _depth);
				if(Picked_move.move.to == Picked_move.move.from) break; //mate or stalemate
				x = FromTo(x, Picked_move.move);
				bestmove = ctos(Picked_move.move.from).append(ctos(Picked_move.move.to));
				std::cout << " " << bestmove << ((x.key >> 28 & 7ULL) ? Promoting_str[x.key >> 28 & 7ULL] : ' ');
			}*/
			std::cout << std::endl;
			x = _x; // go back
			ct = clock() - ct;
			std::cout << "info string " << ((double)ct)/CLOCKS_PER_SEC << "sec" << "  hash: " << Hash_Position(&x) << std::endl;
			std::cout << left << "  " << right << std::endl;
			fflush(stdout);

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
			//fflush(stdout);
		}
		else if(strcmp(t, "ucinewgame") == 0){
			x = {};
			//x.key ^= 1ULL << 8;
		}
		else if(strcmp(t, "eval") == 0){
			t = strtok(NULL, " \n");
			if(strcmp(t, "current") == 0){
				std::cout << "value " << Evaluate(&x) << std::endl;
			}
			else if(t){
				ApplyFen(&x, t);
				std::cout << "value " << Evaluate(&x) << std::endl;
			}
			else{
				std::cout << "value " << Evaluate(&x) << std::endl;
			}	
		}
		else if(strcmp(t, "perft") == 0){
			clock_t ct = clock();
			t = strtok(NULL, " \n");
			if(t == NULL) continue;
			moves m;
			movegen(&m, x);
			u64 move_count = 0;
			if(std::stoi(t) == 1){
				for(int i = 0; i < m.size; i++){
					std::cout << ctos(m.moves[i].from) << ctos(m.moves[i].to) << Promoting_str[(m.moves[i].move_type > 2 && m.moves[i].move_type < 7) ? m.moves[i].move_type - 2 : 0] << " 1" << std::endl;
				}
				std::cout << "move" << m.size << std::endl;
			}
			else{
				for(int i=0; i < m.size; i++){
					make_move(&x, m.moves[i]);
					u64 Current_Perft = perft(x, std::stoi(t) - 1);
					undo_move(&x, m.moves[i]);
					std::cout << ctos(m.moves[i].from) << ctos(m.moves[i].to) << Promoting_str[(m.moves[i].move_type > 2 && m.moves[i].move_type < 7) ? m.moves[i].move_type - 2 : 0]
					  	<< " " << Current_Perft << std::endl;
					move_count += Current_Perft;
				}
				std::cout << "depth " << std::stoi(t) << " total " << move_count << std::endl;
			}
			ct = clock() - ct;
			std::cout << ((double)ct)/CLOCKS_PER_SEC << std::endl;
		}
	/*	else if(strcmp(t, "make_move") == 0){
			t = strtok(NULL, " \n");
			int i = std::stoi(t);
			t = strtok(NULL, " \n");
			ApplyFen(&x, t);
			moves m;
			movegen(&m, x);
			make_move(&x, m.moves[i]);
			//for(int k = 0; k < m.size; k++) PrintBitBoard(*(&x.occupied + k));
			std::cout << "from " << (int)m.moves[i].from << " to " << (int)m.moves[i].to << std::endl;
		}*/
		fclose(fptr);
		//fflush(stdout);
	}
}
