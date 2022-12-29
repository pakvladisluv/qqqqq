#include "matrix_input.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

void synchronize(int total_threads)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
    static volatile int threads_in = 0;
    static volatile int threads_out = 0;
    pthread_mutex_lock(&mutex);
    threads_in++;
    if (threads_in >= total_threads) {
        threads_out = 0;
        pthread_cond_broadcast(&condvar_in);
    } else {
        while (threads_in < total_threads) {
            pthread_cond_wait(&condvar_in, &mutex);
        }
    }
    threads_out++;
    if (threads_out >= total_threads) {
        threads_in = 0;
        pthread_cond_broadcast(&condvar_out);
    } else {
        while (threads_out < total_threads) {
            pthread_cond_wait(&condvar_out, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
}

int decomp(double *matrix, double *Q, double *coss, double *sinn, double *x, double *y, double *r, double *tmp, /*double *ans,*/ int n, int t, int total) {
//    int L = thread_num * n / total;
//    int R = (thread_num + 1) * n / total;
//    double x, y, r;
//if(t == 0){
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            x[i * n + j] = matrix[i + i * n];
            y[i * n + j] = matrix[i + j * n];
            r[i * n + j] = sqrt(x[i * n + j] * x[i * n + j] + y[i * n + j] * y[i * n + j]);
            if (fabs(r[i * n + j]) < 1e-100) {
                continue;
            }
            coss[i * n + j] = x[i * n + j] / r[i * n + j];
            sinn[i * n + j] = -1 * (y[i * n + j] / r[i * n + j]);
            matrix[i * n + i] = r[i * n + j];
            matrix[j * n + i] = 0.0;
//            synchronize(total);
            for (int k = i + 1; k < n; k++) {
                x[i * n + j] = matrix[i * n + k];
                y[i * n + j] = matrix[j * n + k];
                matrix[i * n + k] = x[i * n + j] * coss[i * n + j] - y[i * n + j] * sinn[i * n + j];
                matrix[j * n + k] = x[i * n + j] * sinn[i * n + j] + y[i * n + j] * coss[i * n + j];
            }
//            synchronize(total);
            for (int k = 0; k < n; k++) {
                x[i * n + j] = Q[k * n + i];
                y[i * n + j] = Q[k * n + j];
                Q[k * n + i] = x[i * n + j] * coss[i * n + j] - y[i * n + j] * sinn[i * n + j];
                Q[k * n + j] = x[i * n + j] * sinn[i * n + j] + y[i * n + j] * coss[i * n + j];
            }
//            synchronize(total);
        }
    }
    if (fabs(matrix[n - 1 + (n - 1) * n]) < 1e-100) {
        cout << "Матрица вырожденна" << endl;
        return -4;
    }
//}
    synchronize(total);
    for (int i = 0; i < n; i++) {
        for (int j = i + t; j < n; j += total) {
            swap(Q[i * n + j], Q[j * n + i]);
        }
    }
    synchronize(total);
    for (int i = t; i < n; i += total) {  //proizved
        for (int j = n - 1; j >= 0; j--) {
        tmp[j] = Q[j * n + i];
        for (int k = j + 1; k < n; k++) {
            tmp[j] -= matrix[j * n + k] * Q[k * n + i];
        }
        Q[j * n + i] = tmp[j] / matrix[j * n + j];
        }
    }
    synchronize(total);
    return 1;
}

double residual(double *matrix, double *inv, int n) {
  double norm = 0, mx = 0, sum = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        sum += matrix[k + i * n] * inv[j + k * n];
      }
      if (i == j) {
        sum -= 1;
      }
      norm += fabs(sum);
    }
    if (norm > mx) {
      mx = norm;
    }
    norm = 0;
    sum = 0;
  }
  return mx;
}
