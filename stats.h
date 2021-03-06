#ifndef STREAM_CIPHER_STATS_H
#define STREAM_CIPHER_STATS_H

#include <stdint.h>

typedef double real;
#define MAX(a, b)  ( (a)<(b) ? (b):(a) )
#define MIN(a, b)  ( (a)<(b) ? (a):(b) )
#define ABS(x)     ( (x)>0 ? (x):(-(x)) )

double chi_sq(int n, uint32_t observed[], float expected[]);
double chi_sq_pval(int Dof, double Cv);
double normal_cdf(double x, double mean, double stdev);
void calculate_bins(int m, /*OUT*/ int *bin, /*OUT*/ float *e_prob);
real KStest(real *x, int dim);
double chi_sq_uniform(double *vals, unsigned int n, unsigned int b);
double prop_under_alpha(double *vals, unsigned int n, double alpha);

#endif //STREAM_CIPHER_STATS_H
