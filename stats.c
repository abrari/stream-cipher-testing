#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "stats.h"

double chi_sq(int n, float observed[], float expected[]) {
    double cs = 0;
    int i;
    for (i = 0; i < n; i++) {
        cs += (((double) observed[i] - (double) expected[i]) * ((double) observed[i] - (double) expected[i]))
              / (double) expected[i];
    }
    return cs;
}

static double igf(double S, double Z) {
    if (Z < 0.0) {
        return 0.0;
    }
    double Sc = (1.0 / S);
    Sc *= pow(Z, S);
    Sc *= exp(-Z);

    double Sum = 1.0;
    double Nom = 1.0;
    double Denom = 1.0;
    int I;

    for (I = 0; I < 200; I++) {
        Nom *= Z;
        S++;
        Denom *= S;
        Sum += (Nom / Denom);
    }

    return Sum * Sc;
}

double chi_sq_pval(int Dof, double Cv) {
    if (Cv < 0 || Dof < 1) {
        return 0.0;
    }
    double K = ((double) Dof) * 0.5;
    double X = Cv * 0.5;
    if (Dof == 2) {
        return exp(-1.0 * X);
    }

    double PValue = igf(K, X);
    if (isnan(PValue) || isinf(PValue) || PValue <= 1e-8) {
        return 1e-14;
    }

    PValue /= tgamma(K);

    return (1.0 - PValue);
}

double normal_cdf(double x, double mean, double stdev) {
    return 0.5 * erfc(-(x - mean) / (stdev * sqrt(2)));
}

void calculate_bins(int m, /*OUT*/ int *bin, /*OUT*/ float *e_prob) {

    int i = 0, x = 0, bins = 5;
    int mean = m/2;
    int variance = m/4;
    double stdev = sqrt(variance);

    float prob;

    do {
        prob = (float)normal_cdf(x, mean, stdev);
        if (prob >= 0.2 * (i+1)) {
            bin[i] = x;
            i++;
        }
        x++;
    } while(i < bins-1);
    bin[bins-1] = m;

    e_prob[0] = (float)(normal_cdf(bin[0], mean, stdev) - normal_cdf(0, mean, stdev)); // first bin prob
    for (i = 1; i < bins; ++i) {
        e_prob[i] = (float)(normal_cdf(bin[i], mean, stdev) - normal_cdf(bin[i-1] + 1, mean, stdev));
    }

    // for (i = 0; i < bins; ++i) {
    //     printf("%d\t%d\t%f\n", i, bin[i], e_prob[i]);
    // }

}

// ================== from diehard's kstest.c ===================

/*This test is based on a modified Kolmogorov-Smirnov method.
  The test-statistic is (FN(X)-X)**2/(X*(1-X)) (Anderson-Darling)
  where X is a uniform under null hypothesis. FN(X) is the empirical
  distribution of X.*/

/*c.d.f of Anderson-Darling statistic (a quick algorithm)*/
real AD(real z) {
    if (z < .01) return 0;

    if (z <= 2) {
        return 2 * exp(-1.2337 / z) * (1 + z / 8 - .04958 * z * z / (1.325 + z)) / sqrt(z);
    }

    if (z <= 4) return 1 - .6621361 * exp(-1.091638 * z) - .95095 * exp(-2.005138 * z);

    if (4 < z) return 1 - .4938691 * exp(-1.050321 * z) - .5946335 * exp(-1.527198 * z);

    return -1;       /*error indicator*/
}

int fcmpr(const void *f1, const void *f2)
{
    real *u1=(real*)f1, *u2=(real*)f2;

    if( *u1<*u2 ) return -1;
    if( *u1==*u2 ) return 0;

    return 1;
}

real KStest(real *x, int dim) {

    int L[10][8] = {{40, 46, 37, 34,  27, 24, 20, 20},
                    {88, 59, 43, 37,  29, 27, 20, 22},
                    {92, 63, 48, 41,  30, 30, 25, 24},
                    {82, 59, 42, 37,  26, 28, 26, 22},
                    {62, 48, 33, 30,  23, 23, 22, 18},
                    {49, 34, 22, 20,  16, 17, 17, 12},
                    {17, 17, 7,  8,   4,  7,  5,  1},
                    {40, 18, 19, 14,  16, 13, 10, 9},
                    {59, 20, 10, 4,   1,  1,  0,  -1},
                    {41, 43, 36, 112, 15, 95, 32, 58}};
    int i, m = MIN(dim - 2, 8) - 1;
    real pvalue, tmp, sum = 0, z = -dim * dim, epsilon = pow(10, -20);

    qsort(x, dim, sizeof(double), fcmpr);

    for (i = 0; i < dim; ++i) {
        tmp = x[i] * (1 - x[dim - 1 - i]);
        tmp = MAX(epsilon, tmp);
        z -= (2 * i + 1) * log(tmp);
    }

    z /= dim;
    pvalue = 1 - AD(z);

    /*for(i=0; i<10; ++i) sum+=L[i][m]*sp(p, i)*.0001;

    if( dim>10 ) sum*=10./dim;

    return p+sum;???*/

    return pvalue;
}


double chi_sq_uniform(double *vals, unsigned int n, unsigned int b) {
    //double *bin = malloc(sizeof(double)*b);
    unsigned int i, k, *o_freq = (unsigned int*) calloc(sizeof(unsigned int), b);
    double divisor = 1.0/b, e_freq = n * divisor;
//    bin[b-1] = 1.0;
//    for(i=b-1; i; i--)
//        bin[i] = b[i+1] - 1.0/b;

    for(i=0; i<n; i++) {
        k = (int)(vals[i]/divisor);
        o_freq[k]++;
    }

    double cs = 0.0;
    for(i=0; i<b; i++) {
        cs += ((double) o_freq[i] - e_freq) * ((double) o_freq[i] - e_freq);
    }
    cs /= e_freq;

    //printf("cs = %f\n", cs);

    return (chi_sq_pval(b-1, cs));
}

double prop_under_alpha(double *vals, unsigned int n, double alpha) {
    unsigned int i, freq=0;

    for(i=0; i<n; i++)
        if (vals[i]<=alpha) freq++;

    return (freq/(double)n);
}








