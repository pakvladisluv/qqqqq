#pragma once
#include <iostream>
using namespace std;

int decomp(double *, double *, double *, double *, double*, double *, double *, double *, int, int, int);
double residual(double *mat, double *inv, int n);
void synchronize(int);
