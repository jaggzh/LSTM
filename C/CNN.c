// CNN.c    -- NAND gate
#include "matrix.h"
#include "printing.h"
int main(void) {
  matrix *in      = make_matrix(4, 4), *out = make_matrix(4, 1),
         *weights = matrix_initialize(random_long_double, 4, 1),
         *errors  = NULL, *answer = NULL;
  long double input[4][4]   = {{0.0, 0.0, 1.0, 1.0},
                               {0.0, 1.0, 1.0, 1.0},
                               {1.0, 0.0, 1.0, 1.0},
                               {1.0, 1.0, 1.0, 1.0}};
  long double output[4][1]  = {{1.0},
                               {1.0},
                               {1.0},
                               {0.0}};
  matrix_push_all(in,      (long double *)input);
  matrix_push_all(out,     (long double *)output);
  puts("Input:");   print_matrix(in); puts("Output:"); print_matrix(out);
  puts("Weights:"); print_matrix(weights);
  for (unsigned int epoch = 0; epoch < 60000; epoch++) {
    answer  = matrix_sigmoid(dot_product(matrix_copy(in),
                                         matrix_copy(weights)));
    if (epoch == 59999) {puts("Feedforward:"); print_matrix(answer);}
    errors  = fold(2, multiply, sigmoid_derivative(answer),
                                fold(2, minus, matrix_copy(out), 
                                               matrix_copy(answer)));
    if (epoch == 59999) {puts("Feedback:");    print_matrix(errors);}
    weights = fold(2, add, weights, 
                           dot_product(transpose(matrix_copy(in)), 
                                       errors));
    if (epoch == 59999) {puts("New weights:"); print_matrix(weights);}
  } in = destroy_matrix(in); out = destroy_matrix(out);
    weights = destroy_matrix(weights);
    return 0;
}