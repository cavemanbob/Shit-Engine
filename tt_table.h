struct tt{
	u64 hash;
	int val;
	u8 depth;
};
typedef struct tt tt;

const u64 tt_table_size = 1024 * 1024;
tt tt_table[tt_table_size];


tt* get_tt(u64 hash){
	if( tt_table[hash % tt_table_size].hash == hash ){	
		return &tt_table[hash % tt_table_size];
	}
	return NULL;
}

void set_tt(u64 hash, int val, u8 depth){
	const int id = hash % tt_table_size;
	tt_table[id].hash = hash;
	tt_table[id].val = val;
	tt_table[id].depth = depth;
}



