

u64 perft(position *b, int depth){
	if(depth == 1){
		moves  m;
		movegen(&m, b);
		return m.size;
	}
	//if(depth == 0) return 1;
	moves  m;
	movegen(&m, b);
	u64 move_count = 0;
	for(int i=0; i < m.size; i++){
		make_move(b, m.moves[i]);
		move_count += perft(b, depth - 1);
		undo_move(b, m.moves[i]);
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
		move _move = {from, to, pawn, (u8)((strchr(Promoting_str, inp[4]) - Promoting_str) + 2), b.turn};
		make_move(&b, _move);
		return b;
	}
	int piece;
	for(piece=0; piece<12; piece++)
		if(get_bit(b.bitboards[piece] , from)) 
			break;
	if( (piece == PAWN_W || piece == PAWN_B) && (abs((int)from % 8 - (int)to % 8) > 0) && get_bit(occupied, to) == 0){
		move _move = {from, to, pawn, b.turn == WHITE ? en_w : en_b, b.turn};
		make_move(&b, _move);
		return b;
	}
	if( (piece == KING_W || piece == KING_B) && abs(int(from) % 8 - int(to) % 8) > 1){
		move _move;
		if(to == 58) _move = {from, to, king, long_castle_b, b.turn};
		if(to == 62) _move = {from, to, king, short_castle_b, b.turn};
		if(to == 2) _move = {from, to, king, long_castle_w, b.turn};
		if(to == 6) _move = {from, to, king, short_castle_w, b.turn};
		make_move(&b, _move);
		return b;
	}
	piece = piece %6;
	move _move = {from, to, (u8)(piece), only_move, b.turn};
	make_move(&b, _move);
	return b;
}

char * find_pv(position b){
	char *pv_str =(char *)malloc(256 * sizeof(char));
	//ReadableBoard(b);	
	while(1){
		int find = 0;
		moves m;
		movegen(&m, &b);
		for(int i =0; i < m.size; i++){
			make_move(&b, m.moves[i]);
			const u64 pos_hash = hash_position(&b);
			//ReadableBoard(b);
			printf("in hash: %x\n", pos_hash);
			//ReadableBoard(b);
			undo_move(&b, m.moves[i]);
			//std::cout << pos_hash << std::endl;
			if(pos_hash == Game_History[b.move_counter]){
				make_move(&b, m.moves[i]);
				//ReadableBoard(b);
				//printf("x %s",ctos(m.moves[i].from).c_str() );
				//sprintf(pv_str + strlen(pv_str), "%s%s%c ", ctos(m.moves[i].from).c_str(), ctos(m.moves[i].to).c_str(),
						//((m.moves[i].move_type > 1 && 7 > m.moves[i].move_type) ? Promoting_str[m.moves[i].move_type - 2] : ' '));
				find = 1;
				break;
			}
		}
		if(find == 0) break;
	}
	return pv_str;
}



#define SEARCH_DEPTH_LONG_TIME 7
#define SEARCH_DEPTH_LOW_TIME 6
#define SEARCH_DEPTH_VERY_LOW_TIME 5
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
			memset(Game_History, 0, sizeof(u64) * 512);
			Game_History_size = 0;
			t = strtok(NULL, "\n"); // get all str
			ApplyFen(&x, t);
			t = strchr(t, 'm');
			if(t){ // there are "moves" str
				t += 6;
				t = strtok(t, " \n");
				while(t){
					x = notation_move(x, t);
					Flags_History_Size -= 4;
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
			if(_depth == 1 && (_game.wtime < 300 || _game.btime < 300)){
				_depth = 4;
			}
			else if(_depth == 1 && (_game.wtime < 2000 || _game.btime < 2000)){
				_depth = SEARCH_DEPTH_VERY_LOW_TIME;
			}
			else if(_depth == 1 && (_game.wtime < 30000 || _game.btime < 30000)){
				_depth = SEARCH_DEPTH_LOW_TIME;
			}
			else if(_depth == 1){
				_depth = 6;
			}
			Global_depth = _depth;
			//infos
			clock_t ct = clock();
			std::cout << "info depth " << _depth << std::endl;
			scored_move Picked_move = Next(x, _depth);
			make_move(&x, Picked_move.move);
			
			std::cout << "info string val = " << Picked_move.val << std::endl;
			std::cout << "info nodes " << Node_Total << std::endl;
			std::cout << "bestmove " << move_to_str(Picked_move.move) << std::endl;
			/* pv line
			for(int i = 1; i < 64; i++){
				printf(" %s", move_to_str(g_PV[i]));
				if(g_PV[i].from == g_PV[i].to) break;
			}*/
			ct = clock() - ct;
			printf("info string %4lf sec  hash: %x \n", (double)ct/CLOCKS_PER_SEC, hash_position(&x));
			//std::cout << left << "  " << right << std::endl;
			
			fflush(stdout);
		}
		else if(strcmp(t, "d") == 0){
			ReadableBoard(x);
			//print_position_bitboards(&x);
			//print_position_flags(&x);
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
				x.captured_piece = 1;
				int eval_q = ((x.turn == WHITE) ? -1 : +1)  * Quiesce(&x, MIN_SCORE, MAX_SCORE, 0);
				std::cout << "value " << Evaluate(&x) << std::endl;
				std::cout << "q_search " << eval_q << std::endl;
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
			movegen(&m, &x);
			u64 move_count = 0;
			if(std::stoi(t) == 1){
				for(int i = 0; i < m.size; i++){
					std::cout << move_to_str(m.moves[i]) << " 1";
				}
				std::cout << "move" << m.size << std::endl;
			}
			else{
				for(int i=0; i < m.size; i++){
					make_move(&x, m.moves[i]);
					u64 Current_Perft = perft(&x, std::stoi(t) - 1);
					undo_move(&x, m.moves[i]);
					std::cout << move_to_str(m.moves[i]) << " " << Current_Perft;
					move_count += Current_Perft;
				}
				std::cout << "depth " << std::stoi(t) << " total " << move_count << std::endl;
			}
			ct = clock() - ct;
			std::cout << ((double)ct)/CLOCKS_PER_SEC << std::endl;
		}
		else if(strcmp(t, "q") == 0){
			t = strtok(NULL, " \n");
			if(t == NULL) break;
			position q_search_pos;
			ApplyFen(&q_search_pos, t);
			//std::cout << "q_search val: " << -Quiesce(&q_search_pos, MIN_SCORE, MAX_SCORE) << std::endl;

		}
		else if(strcmp(t, "n") == 0){
			t = strtok(NULL, " \n");
			if(t == NULL) break;
			position n_search_pos;
			ApplyFen(&n_search_pos, t);
			//std::cout << "negamax val: " << negamax(&n_search_pos, 5, MIN_SCORE, MAX_SCORE,) << std::endl;
		}
		else if(strcmp(t, "hash") == 0){
			for(int i = 0; i < 64; i++){
				printf("hash%d %x\n", i, Game_History[i]);
			}
		}
		fclose(fptr);
		//fflush(stdout);
	}
}
