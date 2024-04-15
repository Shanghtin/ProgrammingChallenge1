#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MATRIX_SIZE 3
#define NUM_THREADS 3

int matrixA[MATRIX_SIZE][MATRIX_SIZE];
int matrixB[MATRIX_SIZE][MATRIX_SIZE];
int resultMatrix[MATRIX_SIZE][MATRIX_SIZE];
int calculationCount = 0;
pthread_mutex_t mutex;

typedef struct {
    int row;
    int col;
} thread_data_t;

void *multiply(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    int i;
    int partialCalculationCount = 0;

    for (i = 0; i < MATRIX_SIZE; i++) {
        resultMatrix[data->row][data->col] += matrixA[data->row][i] * matrixB[i][data->col];
        partialCalculationCount += matrixA[data->row][i] * matrixB[i][data->col];
    }

    // Lock the mutex before updating the shared variable
    pthread_mutex_lock(&mutex);
    calculationCount += partialCalculationCount;
    // Unlock the mutex after updating
    pthread_mutex_unlock(&mutex);

    return NULL; // Added return statement
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialize matrices A and B
    int i, j;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            matrixA[i][j] = rand() % 10;
            matrixB[i][j] = rand() % 10;
        }
    }

    // Create threads to compute the result matrix
    int thread_count = 0;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            thread_data[thread_count].row = i;
            thread_data[thread_count].col = j;
            pthread_create(&threads[thread_count], NULL, multiply, (void *)&thread_data[thread_count]);
            thread_count++;

            if (thread_count >= NUM_THREADS)
                thread_count = 0;
        }
    }

    // Wait for all threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the result matrix
    printf("Result Matrix:\n");
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", resultMatrix[i][j]);
        }
        printf("\n");
    }

    // Print the sum of all the calculations
    printf("Total calculations: %d\n", calculationCount);

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}