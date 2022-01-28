#include "loisProba.h"


double generer_U01(){
    return rand()/(double)(RAND_MAX);
}

double generer_expo(double lambda){
    return -log(1-generer_U01())/(double)lambda;
}

double generer_normale(double mu, double sigma){
	int precision = 100;
    double s,terme;
    s=0;
    for (int j=0; j<precision; j++){
        s+=generer_U01();
    }
    return terme=mu+(sqrt(12/(double)precision)*(s-precision/(double)2))*sigma;
}
