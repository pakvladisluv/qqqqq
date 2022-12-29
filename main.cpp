#include "matrix_input.h"
#include "reverse.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

#define GIGA_MODIFIER 1e9
#define KILO_MODIFIER 1e3
#define MICRO_MODIFIER 1e-6

#define arg_max 6
#define arg_min 5

using namespace std;

unsigned long long currentTimeNano() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)(t.tv_sec * GIGA_MODIFIER + t.tv_nsec);
}

unsigned long long currentTimeMillis() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)(t.tv_sec * KILO_MODIFIER + t.tv_nsec * MICRO_MODIFIER);
}

typedef struct _ARGS {
    double *a;    // = NULL;
    double *q;    // = NULL;
    double *coss; // = NULL;
    double *sinn; // = NULL;
    double *X;    // = NULL;
    double *Y;    // = NULL;
    double *R;    // = NULL;
    double *tmp;  // = NULL;
    int n;
    int thread_num;
    int total_threads;
} ARGS;

static long int threads_total_time = 0;
pthread_mutex_t mutexx = PTHREAD_MUTEX_INITIALIZER;
bool check = true;

void *Func(void *p_arg) { // rename later
    ARGS *arg = (ARGS *)p_arg;
    unsigned long long time;
    time = currentTimeNano();
    if (decomp(arg->a, arg->q, arg->coss, arg->sinn, arg->X, arg->Y, arg->R,
                         arg->tmp, arg->n, arg->thread_num, arg->total_threads) == -4) {
        check = false;
    }
    pthread_mutex_lock(&mutexx);
    cout << "Time of thread " << arg->thread_num << ": " << time << " ns" << endl;
    time = currentTimeNano() - time;
    threads_total_time += time;
    pthread_mutex_unlock(&mutexx);
    return NULL;
}

int main(int argc, char *argv[]) {
    int n, m, k, po;
    if (argc < arg_min || argc > arg_max) {
        cout << "Неверное количество аргументов" << endl;
        return -1;
    }
    if (atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || atoi(argv[1]) < 0 ||
            atoi(argv[3]) > atoi(argv[2]) || atoi(argv[4]) < 0 || atoi(argv[4]) > 4) {
        cout << "Некорректные значения аргументов" << endl;
        return -1;
    }
    po = atoi(argv[1]);
    n = atoi(argv[2]);
    m = atoi(argv[3]);
    k = atoi(argv[4]);

    if (k == 0 && argc < arg_max) {
        cout << "Некорректные аргументы, не указан файл при k=0" << endl;
        return -1;
    }
    double *matrix = new double[n * n];
    if (matrix == NULL) {
        cout << "Не удалось выделить память" << endl;
        delete[] matrix;
        return -2;
        exit(-2);
    }
    double *q = new double[n * n];
    if (q == NULL) {
        cout << "Не удалось выделить память" << endl;
        delete[] matrix;
        delete[] q;
        return -2;
        exit(-2);
    }

    int rtrn_val = 0; // for func
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j)
                q[j + i * n] = 0;
            else
                q[j + i * n] = 1;
    if (k == 0) {
        ifstream input;
        if (argv[5] != NULL) {
            input.open(argv[5]);
            if (input.fail()) {
                cout << "Указанного файла не существует" << endl;
                delete[] matrix;
                delete[] q;
                return -3;
            }
            if (input.peek() == EOF) {
                cout << "Файл пуст" << endl;
                delete[] matrix;
                delete[] q;
                return -3;
            }
        } else {
            cout << "Не указан файл" << endl;
            return -1;
        }
        rtrn_val = read_file(argv[5], matrix, n);
        if (rtrn_val == (-3)) {
            delete[] matrix;
            delete[] q;
            return -3;
        }
    } else {
        read_matrix(matrix, k, n);
    }
    prnt(matrix, m, n);
    double *coss = new double[n * n];
    double *sinn = new double[n * n];
    double *X = new double[n * n];
    double *Y = new double[n * n];
    double *R = new double[n * n];
    double *tmp = new double[n];

    ARGS *args;
    args = new ARGS[po];
    pthread_t *threads;
    threads = new pthread_t[po];

    for (int i = 0; i < po; i++) {
        args[i].a = matrix;
        args[i].q = q;
        args[i].sinn = sinn;
        args[i].coss = coss;
        args[i].X = X;
        args[i].Y = Y;
        args[i].R = R;
        args[i].tmp = tmp;
        args[i].n = n;
        args[i].thread_num = i;
        args[i].total_threads = po;
    }
    unsigned long long time;
    time = currentTimeNano();
    for (int i = 0; i < po; i++) {
        if (pthread_create(threads + i, 0, Func, args + i)) {
            cout << "Can not create thread " << i << '!' << endl;
            if (matrix != nullptr) {
                delete[] matrix;
            }
            if (q != nullptr) {
                delete[] q;
            }
            if (threads != nullptr) {
                delete[] threads;
            }
            if (args != nullptr) {
                delete[] args;
            }
            return -(arg_min);
        }
    }
    for (int i = 0; i < po; i++) {
        if (pthread_join(threads[i], 0)) {
            cout << "Can not wait thread " << i << '!' << endl;
            if (matrix != nullptr) {
                delete[] matrix;
            }
            if (q != nullptr) {
                delete[] q;
            }
            if (threads != nullptr) {
                delete[] threads;
            }
            if (args != nullptr) {
                delete[] args;
            }
            return -(arg_min);
        }
    }
    time = currentTimeNano() - time;
    cout << "Time: " << time << " ns" << endl;
    if (!check) {
        delete[] matrix;
        delete[] q;
        delete[] sinn;
        delete[] coss;
        delete[] X;
        delete[] Y;
        delete[] R;
        delete[] tmp;
        delete[] args;
        delete[] threads;
        return -4;
    }
    cout << "Inverse: " << endl;
    // prnt(matrix, m, n);
    prnt(q, m, n);
    if (k == 0) {
        rtrn_val = read_file(argv[5], matrix, n);
    } else {
        read_matrix(matrix, k, n);
    }
    cout << "Residual: " << residual(matrix, q, n) << endl;
    delete[] matrix;
    delete[] q;
    // delete[] threads;
    delete[] sinn;
    delete[] coss;
    delete[] X;
    delete[] Y;
    delete[] R;
    delete[] tmp;
    delete[] args;
    return 0;
}
