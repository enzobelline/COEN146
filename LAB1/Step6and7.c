#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double factorial(int x){
    int i = 1;
    double val = 1;
    for (i = x; i>1 ; i--){
        val *= i;
    }
    return val;
}

int main(){
    int linkBandwidth = 200, userBandwidth = 20, nPSusers = 19;
	double tPSusers = 0.10;

    // Step 6
    int nCSusers = linkBandwidth/userBandwidth;
    printf("# of CS Users: %d\n", nCSusers);

    // Step 7
    double pPSusers = tPSusers;
    printf("Probability a PS user is busy transmitting: %f\n",pPSusers);
    
    double pPSusersNotBusy = 1 - pPSusers;
    printf("Probability that one specific other user is not busy is: %f\n",pPSusersNotBusy);

    double c = pPSusers * pow(pPSusersNotBusy, 18);
    printf("Probability that all of the other specific other users are not busy is %f\n",c);

    double d = pPSusers * pow(pPSusersNotBusy, 18);
    printf("Probability one user is busy transmitting: %.3e\n", d);

    double e = 19 * pPSusers * pow(pPSusersNotBusy, 18);
    printf("Probability any one user is busy transmitting: %.3e\n", e);

    double f = pow(pPSusers, 10) * pow(pPSusersNotBusy, nPSusers - 10);
    printf("Probability ten users are busy transmitting: %.3e\n", f);

    double uc = factorial(nPSusers)/ factorial(10) /factorial(nPSusers - 10);
    double g = f * uc;
    printf("Probability any ten users are busy transmitting: %.3e\n", g);

    int count = 11;
    double val = 0;
    for (count; count < 20; ++count){
        double countUsers = pow(pPSusers, count) * pow(pPSusersNotBusy, nPSusers - count);
        double combos = factorial(nPSusers)/ factorial(count) /factorial(nPSusers - count);
        val += countUsers * combos;
    }
    printf("Probability more than ten users are busy transmitting: %.3e\n", val);
}
