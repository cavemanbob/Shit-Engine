#ifndef C_NN

#define C_NN

#ifndef C_MATRIX
#include "c_matrix.h"
#endif


enum net_modes : int{
	normal = 1,
	resnet = 2
};


struct nn{
	matrix **weights;
	matrix **biases;
	matrix **weights_gradient;
	matrix **gradient_dot;
	matrix **biases_gradient;
	matrix **values;
	matrix **errors;
	int layer_num;
	int weights_num;
	int *layer_scheme;
	float lr;
	int activation_function;
	int mode;
	unsigned int resnet_jump;
};
typedef struct nn nn;


nn *create_nn(int scheme_size, int *scheme, const unsigned int act_func, const real_t rand_start, const real_t rand_end){

	nn *n1 = (nn *)malloc( sizeof(nn) );
	n1->layer_num = scheme_size;
	n1->layer_scheme = scheme;
	n1->weights_num = scheme_size - 1;
	
	n1->weights = (matrix **)malloc( n1->weights_num * sizeof(matrix *));
	n1->biases = (matrix **)malloc( n1->weights_num * sizeof(matrix *));

	n1->weights_gradient = (matrix **)malloc( n1->weights_num * sizeof(matrix *));
	n1->gradient_dot = (matrix **)malloc( n1->weights_num * sizeof(matrix *));
	n1->biases_gradient = (matrix **)malloc( n1->weights_num * sizeof(matrix *));

	n1->values = (matrix **)malloc( n1->layer_num * sizeof(matrix *));
	n1->errors = (matrix **)malloc( n1->layer_num * sizeof(matrix *));

	for(int i = 0; i < n1->layer_num - 1; i++){
		n1->weights[i] = create_matrix(scheme[i + 1], scheme[i]);
		n1->biases[i] = create_matrix(scheme[i + 1], 1);

		n1->weights_gradient[i] = create_matrix(scheme[i + 1], scheme[i]);
		n1->gradient_dot[i] = create_matrix(scheme[i + 1], scheme[i]);
		n1->biases_gradient[i] = create_matrix(scheme[i + 1], 1);

		matrix_rand(n1->weights[i], rand_start, rand_end);
		matrix_rand(n1->biases[i], rand_start, rand_end);
		matrix_rand(n1->weights_gradient[i], rand_start, rand_end);
		matrix_rand(n1->biases_gradient[i], rand_start, rand_end);
	}
	for(int i = 0; i < n1->layer_num; i++){
		n1->values[i] = create_matrix(scheme[i], 1);
		n1->errors[i] = create_matrix(scheme[i], 1);
	}
	
	n1->activation_function = act_func;
	n1->mode = normal;

	n1->lr = 0.1f;

	return n1;
}
void create_resnet(nn *n1, const unsigned int jump ){
	n1->resnet_jump = jump;
	n1->mode = resnet;
}

matrix * feed(nn *n1, matrix *m1){
	matrix_cpy(n1->values[0], m1);
	if(n1->mode == normal){
		for(int i = 0; i < n1->weights_num; i++){
			matrix_dot( n1->weights[i], n1->values[i], n1->values[i + 1] );
			matrix_add( n1->values[i + 1], n1->biases[i] );

			if( n1->activation_function == sigmoid){
				matrix_sig( n1->values[i + 1] );
			}
			else if( n1->activation_function == relu){
				matrix_relu( n1->values[i + 1] );
			}
			else{
				printf("no activation function selected\n");
				assert(0);
			}
		}
	}
	else if(n1->mode == resnet){
		for(int i = 0; i < n1->weights_num; i++){
			if(i % n1->resnet_jump == 1 ){
				matrix_add(n1->values[i], n1->values[0]);
			}
			matrix_dot( n1->weights[i], n1->values[i], n1->values[i + 1] );
			matrix_add( n1->values[i + 1], n1->biases[i] );

			if( n1->activation_function == sigmoid){
				matrix_sig( n1->values[i + 1] );
			}
			else if( n1->activation_function == relu){
				matrix_relu( n1->values[i + 1] );
			}
			else{
				printf("no activation function selected\n");
				assert(0);
			}
		}
	}
	
	return n1->values[n1->layer_num - 1];
}

void train(nn *n1, matrix *m1, matrix *mtarget){
	matrix_cpy( n1->errors[n1->weights_num], mtarget);
	matrix_sub( n1->errors[n1->weights_num], feed(n1, m1));
	
	if(n1->mode == normal){
		for(int i = n1->weights_num; i > 0; i--){

			if( n1->activation_function == sigmoid){
				matrix_mul( n1->errors[i], matrix_dsig( n1->values[i]));
			}
			else if( n1->activation_function == relu){
				matrix_mul( n1->errors[i], matrix_drelu( n1->values[i]));
			}
			else{
				printf("no activation function selected\n");
				assert(0);
			}

			//matrix_mul( n1->errors[i], matrix_dsig( n1->values[i]));
			matrix *m_t = matrix_transpose( n1->weights[i - 1] );
			matrix_dot( m_t, n1->errors[i], n1->errors[i - 1] );
			free_matrix(m_t);
		
			matrix *v_t = matrix_transpose( n1->values[i - 1] );
			matrix_dot( n1->errors[i], v_t, n1->gradient_dot[i - 1] );
			matrix_add( n1->weights_gradient[i - 1], n1->gradient_dot[i - 1] );
			free_matrix(v_t);

			matrix_add( n1->biases_gradient[i - 1], n1->errors[i] );
		}
	}
	else if(n1->mode == resnet){
		for(int i = n1->weights_num; i > 0; i--){

			if( n1->activation_function == sigmoid){
				matrix_mul( n1->errors[i], matrix_dsig( n1->values[i]));
			}
			else if( n1->activation_function == relu){
				matrix_mul( n1->errors[i], matrix_drelu( n1->values[i]));
			}
			else{
				printf("no activation function selected\n");
				assert(0);
			}

			matrix *m_t = matrix_transpose( n1->weights[i - 1] );
			matrix_dot( m_t, n1->errors[i], n1->errors[i - 1] );
			free_matrix(m_t);
		
			matrix *v_t = matrix_transpose( n1->values[i - 1] );
			matrix_dot( n1->errors[i], v_t, n1->gradient_dot[i - 1] );
			matrix_add( n1->weights_gradient[i - 1], n1->gradient_dot[i - 1] );
			free_matrix(v_t);

			matrix_add( n1->biases_gradient[i - 1], n1->errors[i] );
		}
	
	
	}

}

void apply_gradient( nn *n1){
	for(int i = 0; i < n1->weights_num; i++){
		matrix_smul( n1->weights_gradient[i], n1->lr);
		matrix_add( n1->weights[i], n1->weights_gradient[i] );
		matrix_fill( n1->weights_gradient[i], 0.0f);

		matrix_smul( n1->biases_gradient[i], n1->lr);
		matrix_add( n1->biases[i], n1->biases_gradient[i] );
		matrix_fill( n1->biases_gradient[i], 0.0f);

	}
}

void print_weights(nn *n1){
	for(int i = 0; i < n1->weights_num; i++){
		print_matrix(n1->weights[i]);
		printf("%d\n\n",i);
	}
}


void print_values(nn *n1){
	for(int i = 0; i < n1->layer_num; i++){
		print_matrix(n1->values[i]);
		printf("%d\n\n",i);
	}
}





#endif
