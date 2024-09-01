
#ifndef C_MATRIX

#define C_MATRIX

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
//#include <immintrin.h>


typedef float real_t;

/*
typedef uint64_t u64;
static uint64_t state = 88798740143389752ULL;

uint64_t rand64() {
    uint64_t x = state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state = x;
}
*/
float randf() {
    u64 x = rand64();
    return (x >> 11) * (1.0f / 9007199254740992.0f);
}



struct matrix{
	real_t *data;
	int size;
	int col;
	int row;
};
typedef struct matrix matrix;

void print_matrix(matrix *m){
	for(int i = 0; i < m->col; i++){
		for(int j = 0; j < m->row; j++){
			printf("%4f ", m->data[i * m->row + j]);
		}
		printf("\n");
	}
	printf("\n");
}

matrix * create_matrix(int col, int row){
	matrix *m1 = (matrix *)malloc(sizeof(matrix));
	if( m1 == NULL) assert(0);
	m1->col = col;
	m1->row = row;
	m1->size = col * row;
	m1->data = (real_t *)malloc( m1->size * sizeof(real_t));
	if( m1->data == NULL) assert(0);
	return m1;
}


matrix * create_ematrix(int col, int row){
	matrix *m1 = (matrix *)malloc(sizeof(matrix));
	if( m1 == NULL) assert(0);
	m1->col = col;
	m1->row = row;
	m1->size = col * row;
	m1->data = (real_t *)calloc( m1->size , sizeof(real_t));
	if( m1->data == NULL) assert(0);
	return m1;
}

void free_matrix(matrix *m1){
	free(m1->data);
	free(m1);
}

void matrix_cpy(matrix *m1, matrix *m2){
	if(m1->row != m2->row || m1->col != m2->col){printf("\nmatrix_cpy unmatched row %d %d col %d %d\n",m1->row, m2->row, m1->col, m2->col);assert(0);}
	
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = m2->data[i];
	}
	//memcpy(m1->data, m2->data, m1->size * sizeof(real_t));
}

void matrix_replace(matrix *m1, matrix *m2){
	free(m1->data);
	m1->data = m2->data;
	m1->col = m2->col;
	m1->row = m2->row;
	m1->size = m2->size;
	free(m2);
}


void matrix_rand(matrix *m1, float s, float e){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = randf() * (e - s) + s;
	}
}

void matrix_fill(matrix *m1, real_t value){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = value;
	}
}


matrix * matrix_transpose(matrix *m1){
	matrix *m2 = create_matrix(m1->row, m1->col);
	for(int i = 0; i < m1->col; i++){
		for(int j = 0; j < m1->row; j++){
			m2->data[ j * m2->row + i] = m1->data[i * m1->row + j];
		}
	}
	return m2;
}

#define get(M, a, b) M->data[(a) * M->row + (b)]
#define simd_get(M, a, b, c) ( (float *) &a[a * c + j])[j % 8]

void matrix_dot(matrix *m1, matrix *m2, matrix *m3){
	if( m1->row != m2->col){printf("\n unmatched row - col\n");assert(0);}
	if( m1->col != m3->col || m2->row != m3->row ){printf("\n dot m3 size error\n");assert(0);}

	// aligned row block_size = sizeof float * block_size
	const int a_block_size = (m1->row + 7) / 8; // aligned block for rows
	const int b_block_size = (m2->col + 7) / 8; // aligned block for rows


	// block_size = simd operation size
	// aligned_alloc(aligned byte, aligneed size per byte)
	__m256 *a = (__m256 *)_aligned_malloc(4 * 8 * m1->col * a_block_size, 32); // float size -> 4 byte * 8 vector size
	__m256 *b = (__m256 *)_aligned_malloc(4 * 8 * m2->row * b_block_size, 32);
	if(a == NULL || b == NULL){printf("MEMORY BLOCK a %p  b %p\n", a, b); assert(0); }

	memset(a, 0, 4 * 8 * m1->col * a_block_size);
	memset(b, 0, 4 * 8 * m2->row * b_block_size);


	//copy to matrixes to aligned region
	for(int i = 0; i < m1->col; i++)
		for(int j = 0; j < m1->row; j++)
			a[i * a_block_size + j / 8][j % 8] = m1->data[i * m1->row + j];

	for(int i = 0; i < m2->col; i++) //transposed copy
		for(int j = 0; j < m2->row; j++){
			b[j * b_block_size + i / 8][i % 8] = get(m2, i, j);
		}





	matrix_fill(m3, 0.0f);

	for(int i = 0; i < m1->col; i++){
		for(int j = 0; j < m2->row; j++){
			__m256 s = {};
			//s = _mm256_setzero_ps();
			for(int k = 0; k < b_block_size; k++){
				s += a[i * a_block_size + k] * b[j * b_block_size + k];
			}
			for(int l = 0; l < 8; l++)
				get(m3, i, j) += s[l];
		}
	}


	_aligned_free(a);
	_aligned_free(b);
}



void matrix_rdot(matrix *m1, matrix *m2, matrix *  m3){
	if( m1->row != m2->col){printf("\n unmatched row - col\n");assert(0);}
	if( m1->col != m3->col || m2->row != m3->row ){printf("\n dot m3 size error\n");assert(0);}

	matrix_fill(m3, 0.0f);
	
	for(int i = 0; i < m1->col; i++){
		for(int j = 0; j < m2->row; j++){
			for(int k = 0; k < m2->col; k++){
				get(m3, i, j) += get(m1, i, k) * get(m2, k, j);
			}
		}
	}
}

void matrix_add(matrix *m1, matrix *m2){ // add m2 to m1
	if(m1->row != m2->row || m1->col != m2->col){printf("\n unmatched row - col\n");assert(0);}
	for(int i = 0; i < m1->size; i++){
		m1->data[i] += m2->data[i];
	}
}

void matrix_sadd(matrix *m1, real_t v){ // add m2 to m1
	for(int i = 0; i < m1->size; i++){
		m1->data[i] += v;
	}
}

void matrix_sub(matrix *m1, matrix *m2){ // add m2 to m1
	if(m1->row != m2->row || m1->col != m2->col){printf("\n unmatched row - col\n");assert(0);}
	for(int i = 0; i < m1->size; i++){
		m1->data[i] -= m2->data[i];
	}
}

void matrix_mul(matrix *m1, matrix *m2){ // add m2 to m1
	if(m1->row != m2->row || m1->col != m2->col){printf("\n unmatched row - col\n");assert(0);}
	for(int i = 0; i < m1->size; i++){
		m1->data[i] *= m2->data[i];
	}
}

void matrix_smul(matrix *m1, real_t value){ // add m2 to m1
	for(int i = 0; i < m1->col; i++){
		for(int j = 0; j < m1->row; j++){
			m1->data[i * m1->row + j] *= value;
		}
	}
}


//#########################################
//
//			Activation Functions
//
//#########################################

enum act_fval : int{
	sigmoid = 1,
	relu = 2
};

matrix * matrix_sig(matrix *m1){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = 1 / (1 + exp(-m1->data[i]));
	}
	return m1;
}

matrix * matrix_dsig(matrix *m1){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = m1->data[i] * (1 - m1->data[i]);
	}
	return m1;
}

inline real_t max(real_t x, real_t y){
	return x > y ? x : y;
}

matrix * matrix_relu(matrix *m1){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = m1->data[i] > 0 ? m1->data[i] : 0;
	}
	return m1;
}

matrix * matrix_drelu(matrix *m1){
	for(int i = 0; i < m1->size; i++){
		m1->data[i] = m1->data[i] > 0 ? 1 : 0;
	}
	return m1;
}


// end proc activation funcs

void check_matrix(matrix *m){
	real_t h = m->data[0];
	m->data[0] = 4.0f;
	m->data[0] = h;
	printf("OK\n");
}

void matrix_rear(matrix *m1, float *ar){
	free(m1->data);
	m1->data = ar;
}

int matrix_max_id(matrix *m){
	real_t max_val = m->data[0];
	int k = 0;
	for(int i = 1; i < m->size; i++){
		if( m->data[i] > max_val){
			max_val = m->data[i];
			k = i;
		}
	}
	return k;
}


real_t matrix_maxval(matrix *m){
	real_t max_val = m->data[0];
	for(int i = 1; i < m->size; i++){
		if( m->data[i] > max_val){
			max_val = m->data[i];
		}
	}
	return max_val;
}

matrix * matrix_resize(matrix * m1, size_t ncol, size_t nrow){
	matrix * r = create_ematrix(ncol, nrow);

	for(int i = 0; i < ncol && i < m1->col; i++){
		for(int j = 0; j < nrow && j < m1->row; j++){
			get(r, i, j) = get(m1, i , j);
		}
	}
	free_matrix(m1);
	return r;
}


/*###################################
 *
 * 			CONV MATRIXES
 *
 *###################################
 */

enum conv_modes : int{
	gray = 1,
	colorful = 2
};

real_t horizontal_kernel[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1 };
real_t vertical_kernel[9] = {1, 1, 1, 0, 0, 0, -1, -1, -1 };
real_t unit_kernel[1] = {1};
matrix sh_kernel = {
	horizontal_kernel,9, 3, 3,
};
matrix sv_kernel = {
	vertical_kernel,9, 3, 3,
};
matrix sunit_kernel = {
	unit_kernel, 1, 1, 1
};

matrix * h_kernel = &sh_kernel;
matrix * v_kernel = &sv_kernel;
matrix * u_kernel = &sunit_kernel;


//TODO: FIX CONV -> it looks correct to me !
matrix * matrix_conv(matrix *m, matrix *kernel, int stride, int pad, int mode){
	if(mode == gray){
		const int out_col = (int) ((m->col + 2 * pad - kernel->col) / stride) + 1;
		const int out_row = (int) ((m->row + 2 * pad - kernel->row) / stride) + 1;
		matrix *mp = create_ematrix( m->col + 2 * pad, m->row + 2 * pad ); //mp matrix padded
		matrix *out = create_ematrix( out_col, out_row );
	
		//printf("col %d, row %d\n", m->col, m->row);
		for(int i = 0; i < m->col; i++){
			for(int j = 0; j < m->row; j++){
				get(mp, i + pad, j + pad) = get(m, i ,j);
			}
		}
		for(int i = 0; i < out_col; i++){
			for(int j = 0; j < out_row; j++){
				//printf("i %d  j %d\n",i ,j);
				for(int ki = 0; ki < kernel->col; ki++){
					for(int kj = 0; kj < kernel->row; kj++){
						//printf("i %d j %d  imp %d  jmp %d  ki %d  kj %d\n", i, j, i * stride + ki, j * stride + kj, ki, kj);
						//printf("%lf * %lf \n", get(mp, i * stride + ki, j * stride + kj), get(kernel, ki, kj));
						get(out, i, j) += get(mp, i * stride + ki, j * stride + kj) * get(kernel, ki, kj);
					}
				}
				//printf("r = %lf\n\n\n", get(out, i, j));
			}
		}
		free_matrix(mp);
		return out;
	}
	return NULL;
}

matrix * matrix_concate(matrix * a, matrix * b){
	matrix *r = create_matrix(a->size + b->size, 1);

	for(int i = 0; i < a->size; i++){
		r->data[i] = a->data[i];
	}

	for(int i = 0; i < b->size; i++){
		r->data[a->size + i] = b->data[i];
	}
	return r;
}


#undef get

#endif
