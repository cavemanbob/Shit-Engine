#define EXACT 1
#define LOWER_BOUND 2
#define UPPER_BOUND 3
struct tt_entry{
	u64 hash;
	int val;
	u8 depth;
	u8 flag; //
};
typedef struct tt_entry tt_entry;

const u64 tt_size = 1024 * 1024;
tt_entry tt[tt_size];


inline tt_entry* get_tt_entry(u64 hash){
	return &tt[hash % tt_size];
}

tt_entry* set_tt_entry(u64 hash, int val, u8 depth){
	const int id = hash % tt_size;
	tt[id].hash = hash;
	tt[id].val = val;
	tt[id].depth = depth;
	return &tt[id];
}



