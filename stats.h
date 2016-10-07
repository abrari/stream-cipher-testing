#ifndef STREAM_CIPHER_STATS_H
#define STREAM_CIPHER_STATS_H

double chi_sq(int n, float observed[], float expected[]);
double chi_sq_pval(int Dof, double Cv);
double normal_cdf(double x, double mean, double stdev);
void calculate_bins(int m, /*OUT*/ int *bin, /*OUT*/ float *e_prob);

#endif //STREAM_CIPHER_STATS_H
