#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

void prnt(double *matrix, int m, int n) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            if (j < m - 1)
                if (matrix[j + i * n] < 0)
                    cout << scientific << setprecision(3) << " " << matrix[j + i * n];
                else
                    cout << scientific << setprecision(3) << "  " << matrix[j + i * n];
            else if (matrix[j + i * n] < 0)
                cout << scientific << setprecision(3) << " " << matrix[j + i * n]
                         << endl;
            else
                cout << scientific << setprecision(3) << "  " << matrix[j + i * n]
                         << endl;
}

void read_matrix(double *matrix, int k, int n) {
    if (k == 1) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[j + i * n] = n - max(i, j);
            }
        }
    } else if (k == 2) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[j + i * n] = max(i, j) + 1;
            }
        }
    } else if (k == 3) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[j + i * n] = abs(i - j);
            }
        }
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[j + i * n] = 1 / (i + j + 1.0);
            }
        }
    }
}

int read_file(char *file_name, double *matrix, int n) {
    ifstream file;
    file.open(file_name);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int numb;
            char ch;
            file >> ch;
            if (int(ch) == 45) {
                file >> ch;
                numb = int(ch) - int('0');
                numb *= -1;
            } else
                numb = int(ch) - int('0');
            if (numb > 9) {
                cout << "В файле есть символы" << endl;
                return -3;
            }
            matrix[j + i * n] = numb;
            if (file.eof()) {
                cout << "Недостаточно элементов" << endl;
                exit(-3);
                return -3;
            }
        }
    }
    file.close();
    return 0;
}
