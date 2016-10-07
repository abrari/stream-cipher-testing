#include <math.h>
#include <stdio.h>

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

void calculate_bins(int m, /*OUT*/ int *bin, /*OUT*/ double *e_prob) {

    const float threshold = 0.001;
    const int bins = 5;

    // find leftmost from center
    int mean = m/2;
    int variance = m/4;
    double stdev = sqrt(variance);
    int x = mean;
    double prob;
    do {
        prob = normal_cdf(x, mean, stdev);
        // printf("%d\t%f\n", x, prob);
        x--;
    } while(prob >= threshold);
    x++;

    // calculate left and right
    int left, right, binwidth, i;
    left = x;
    right = mean + (mean - x);
    binwidth = (right - left) / bins;

    // calculate bin boundaries
    for (i = 0; i < bins-1; ++i) {
        bin[i] = left + binwidth * (i + 1);
    }
    bin[bins-1] = m; // last bin = m

    // calculate expected probabilities
    e_prob[0] = normal_cdf(bin[0], mean, stdev) - normal_cdf(0, mean, stdev); // first bin prob
    for (i = 1; i < bins; ++i) {
        e_prob[i] = normal_cdf(bin[i], mean, stdev) - normal_cdf(bin[i-1] + 1, mean, stdev);
    }

}









