#include <math.h>

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

