// LSTM.c
#include "LSTM.h"

LSTM_type *make_LSTM(unsigned int t, unsigned int rows, unsigned int columns) {
  LSTM_type *LSTM = NULL;
  LSTM = malloc(sizeof(LSTM_type));
  assert(LSTM);
  srand(time(NULL));
  //srand(1);

  // Empty input/outputs to initialize (Xt_i, Yt_k):
  LSTM_initialize(LSTM, Xt_i, Yt_k, zero, t, columns, rows);
  LSTM_initialize(LSTM, Yt_k, GATES_BEGIN, zero, t, rows, columns);

  LSTM_initialize(LSTM, GATES_BEGIN, GATES_END, 
                  one, 2, columns, rows);
  LSTM_initialize(LSTM, INPUT_WEIGHTS_BEGIN, INPUT_WEIGHTS_END, 
                  random_long_double, 1, rows, columns);
  LSTM_initialize(LSTM, HIDDEN_WEIGHTS_BEGIN, HIDDEN_WEIGHTS_END, 
                  random_long_double, 1, rows, rows);
  LSTM_initialize(LSTM, CELL_WEIGHTS_BEGIN, CELL_WEIGHTS_END, 
                  random_long_double, 1, rows, columns);
  LSTM_initialize(LSTM, ERRORS_BEGIN, ERRORS_END, 
                  zero, 2, columns, rows);
  LSTM_initialize(LSTM, INPUT_UPDATES_BEGIN, INPUT_UPDATES_END, 
                  zero, 1, rows, columns);
  LSTM_initialize(LSTM, HIDDEN_UPDATES_BEGIN, HIDDEN_UPDATES_END, 
                  zero, 1, rows, rows);
  LSTM_initialize(LSTM, CELL_UPDATES_BEGIN, CELL_UPDATES_END, 
                  zero, 1, rows, columns);
  return LSTM;
}

LSTM_type *destroy_LSTM(LSTM_type *LSTM) {
  for (index_type tensor = 0; tensor < LSTM_SIZE; tensor++) {
    for (unsigned int time = 0; time < LSTM->tensor[tensor].time; time++) {
      LSTM->tensor[tensor].matrix[time] = destroy_matrix(LSTM->tensor[tensor].matrix[time]);
    } free(LSTM->tensor[tensor].matrix);
      LSTM->tensor[tensor].matrix = NULL;
  }   free(LSTM);
      return NULL;
}

void LSTM_initialize(LSTM_type *LSTM, index_type begin, index_type end, long double (*init)(long double), unsigned int time, unsigned int rows, unsigned int columns) {
  for (index_type tensor = begin; tensor < end; tensor++) {
    LSTM->tensor[tensor].time = time;
    LSTM->tensor[tensor].matrix = calloc(1, sizeof(matrix_type *) + sizeof(matrix_type) * time);
    assert(LSTM->tensor[tensor].matrix);
    for (unsigned int t = 0; t < time; t++) {
      LSTM->tensor[tensor].matrix[t] = make_matrix(rows, columns);
      matrix_for_each(init, LSTM->tensor[tensor].matrix[t]);
    }
  }
}

matrix_type *first(LSTM_type *LSTM, index_type tensor) {
  return matrix_copy(LSTM->tensor[tensor].matrix[LSTM->tensor[tensor].time - 1]);
}

matrix_type *second(LSTM_type *LSTM, index_type tensor) {
  return matrix_copy(LSTM->tensor[tensor].matrix[LSTM->tensor[tensor].time - 2]);
}

matrix_type *third(LSTM_type *LSTM, index_type tensor) {
  return matrix_copy(LSTM->tensor[tensor].matrix[LSTM->tensor[tensor].time - 3]);
}

void push(LSTM_type *LSTM, index_type tensor, matrix_type *matrix) {
  LSTM->tensor[tensor].matrix = realloc(LSTM->tensor[tensor].matrix, sizeof(matrix_type *) + sizeof(matrix_type) * (LSTM->tensor[tensor].time + 1));
  assert(LSTM->tensor[tensor].matrix);
  LSTM->tensor[tensor].matrix[LSTM->tensor[tensor].time++] = matrix;
}

matrix_type *pop(LSTM_type *LSTM, index_type tensor) {
  matrix_type *matrix = first(LSTM, tensor);
  LSTM->tensor[tensor].matrix = realloc(LSTM->tensor[tensor].matrix, sizeof(matrix_type *) + sizeof(matrix_type) * LSTM->tensor[tensor].time - 1);
  assert(LSTM->tensor[tensor].matrix);
  return matrix;
}

void push_all(LSTM_type *LSTM, index_type tensor, long double *steps) {
  unsigned int time = LSTM->tensor[tensor].time, rows, columns;
  for (unsigned int n = 0; n < time; n++) {
    rows = LSTM->tensor[tensor].matrix[n]->rows;
    columns = LSTM->tensor[tensor].matrix[n]->columns;
    for (unsigned int row = 0; row < rows; row++) {
      for (unsigned int column = 0; column < columns; column++) {
        LSTM->tensor[tensor].matrix[n]->matrix[row][column] = *(steps + n * rows * columns + row * columns + column);
      }
    }
  }
}

void LSTM_copy_last_matrix_to_beginning(LSTM_type *LSTM, index_type beginning, index_type end) {
  for (unsigned int tensor = beginning; tensor < end; tensor++) {
    push(LSTM, tensor, matrix_copy(LSTM->tensor[tensor].matrix[0]));
  }
}
