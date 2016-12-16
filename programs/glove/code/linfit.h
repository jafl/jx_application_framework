#ifndef Linfit_h_
#define Linfit_h_

void linlsq2(int rows,double *x,double *y,double *sx, double *sy,double *a,double *aerr,double *b,double *berr,double *c);
void quad_min_fit(int rows,double *x,double *y,double *sx, double *sy,double *a,double *aerr,double *b,double *berr,double *c, double *std::cerr, double *chi2);

#endif /*Linfit_h_*/
