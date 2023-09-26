/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


// Define Locks, Condition variables, and so on here
pthread_cond_t empty, full;
int fill = 0;
int count = 0;
int use = 0;
pthread_mutex_t  putLock = PTHREAD_MUTEX_INITIALIZER;
counter_t *prod_count;
counter_t *con_count;

// Bounded buffer put() get()
int put(Matrix *value) {
    bigmatrix[fill] = value;
    fill = (fill++) % BOUNDED_BUFFER_SIZE;
    count++;
    return 0;
}

Matrix * get() {
    Matrix *tmp = bigmatrix[use];
    use = (use++) % BOUNDED_BUFFER_SIZE;
    count--;
    return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {
    int i;
    for (i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&putLock);
        while (count == MAX) {
            pthread_cond_wait(&empty, &putLock); // p3
        }

        put(GenMatrixRandom());
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&putLock);
    }
    increment_cnt(prod_count);
    return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {
    int i;
    for (i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&putLock);
        while (count  == 0) {
            pthread_cond_wait(&fill, &putLock); // p3
        }

        Matrix *m1 = get();
        Matrix *m2 = get();
        Matrix *m3 = MatrixMultiply(m1,m2);
        DisplayMatrix(m1,stdout);
        printf("    X\n");
        DisplayMatrix(m2,stdout);
        printf("    =\n");
        DisplayMatrix(m3,stdout);
        printf("\n");
        if (m3 != NULL) {
            FreeMatrix(m3);
            FreeMatrix(m2);
            FreeMatrix(m1);
            m1=NULL;
            m2=NULL;
            m3=NULL;
        }
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&putLock);
    }
    increment_cnt(con_count);
    return NULL;
}
