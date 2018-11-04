#include <math.h>
#include "linfit.h"
#include <stdio.h>

#define ITMAX  100
#define CGOLD  0.3819660
#define ZEPS   1.0e-10
#define TOLL   1.0e-10

enum {
	linearFit=1,
	exponentialFit,
	quadraticFit,
	quadErrorsCmin,
	quadErrorsBmin,
	quadErrorsAmin,
	powerlawFit
};

enum {
	C_only=1,
	A_and_B
};

enum {
  MOVE_AWAY=1,
  MOVE_BACK
};

void linlsq1(int rows,double *x,double *y,double *sy,double *a,double *aerr,double *b,double *berr,double *c);
double chisq2(double B);
void paramin(double ax, double bx, double cx, double *ymin, int type);
double variance(double data[]);

double chi2err(double xtemp);
double root(double btemp);

double *X, *Y, *SX, *SY, A, B, C, Chi2, realC, Aerr, Berr, Cerr, realChi2;
int ROWS, globalType, chi2Type;

void linlsq1
	(
	int rows,
	double *x,
	double *y,
	double *sy,
	double *a,
	double *aerr,
	double *b,
	double *berr,
	double *c
	)
{
	double *weight;
	double num;
	double *t;
	double stt;
	double sig;
	double avgx;
	double *W;
	double c2;
	double A;
	int i;
	double sytest;

	weight = (double *)malloc(rows*sizeof(double));
	W = (double *)malloc(rows*sizeof(double));

	for ( i = 0; i < rows; i++)
		{
		weight[i]=1/(sy[i]*sy[i]);
		}

	num = 0;
	for ( i = 0; i < rows; i++)
		{
		num+=weight[i];
		}

	avgx = 0;
	for ( i = 0; i < rows; i++)
		{
		avgx += weight[i]*x[i];
		}
	avgx /=num;

	t = (double *)malloc(rows*sizeof(double));
	for ( i = 0; i < rows; i++)
		{
		t[i]=(x[i]-avgx)/sy[i];
		}

	stt=0;
	for ( i = 0; i < rows; i++)
		{
		stt += t[i]*t[i];
		}

	*b = 0;
	for ( i = 0; i < rows; i++)
		{
		*b+=(t[i]*y[i]/sy[i]);
		}
	*b/=stt;

	*a=0;
	for ( i = 0; i < rows; i++)
		{
		*a+=weight[i] * (y[i]- *b * x[i]);
		}
	*a/=num;

	*aerr=0;
	for ( i = 0; i < rows; i++)
		{
		*aerr+=(weight[i]*weight[i])*(x[i]*x[i]);
		}
	*aerr=*aerr/(num*stt);
	*aerr +=1;
	*aerr/=num;
	*aerr=sqrt(*aerr);

	*berr=sqrt(1.0/stt);

	*c = 0;
	for ( i = 0; i < rows; i++)
		{
		*c += pow(((y[i] - *a - *b * x[i])/sy[i]),2);
		}

	sytest = 1;
	for ( i = 0; i < rows; i++)
		{
		if (sy[i]!=1) sytest = 0;
		}

	if (sytest)
		{
		sig = sqrt(*c/rows);
		*aerr = *aerr*sig;
		*berr = *berr*sig;
		}
}

double chisq2(double Bt) {
	double *err, temp1, temp2, *W;
	int i;

	err = (double *)malloc(ROWS*sizeof(double));
	for ( i = 0; i < ROWS; i++) err[i]=pow(SX[i],2)*Bt*Bt + pow(SY[i],2);

	W = (double *)malloc(ROWS*sizeof(double));
	for ( i = 0; i < ROWS; i++) W[i]=1/err[i];

	temp1 = 0;
	for ( i = 0; i < ROWS; i++) temp1 += W[i] * (Y[i]-Bt*X[i]);

	temp2 = 0;
	for ( i = 0; i < ROWS; i++) temp2 += W[i];

	A = temp1/temp2;

	C = 0;
	for ( i = 0; i < ROWS; i++) C += (pow(Y[i] - A - Bt*X[i],2)/err[i]);

	return C;
}

void paramin(double ax, double bx, double cx, double *xmin, int type){
  double oldstep, x, w, v, fx, fw, fv, middle, tol2, ymin, r, q;
  double p, steptemp, tol1, step, low, high, u, fu;
  int iter;

  oldstep = 0.0;
  x = bx;
  w = bx;
  v = bx;

  switch (type){
  case linearFit:
  case exponentialFit:
	fx = chisq2(bx);
	break;
  case quadraticFit:
	fx = chisq2_quad(bx);
	break;
  case quadErrorsCmin:
	fx = chisq2_quad_c_error(bx);
	break;
  case quadErrorsBmin:
	fx = chisq2_quad_b_error(bx);
	break;
  case quadErrorsAmin:
	fx = chisq2_quad_a_error(bx);
	break;
  }

  fw = fx;
  fw = fx;
  if (ax<cx) {
	low = ax;
	high = cx;
  } else {
	low = cx;
	high = ax;
  }
  iter = 1;
  while (iter<= ITMAX) {
	middle=0.5*(low+high);
	tol1=TOLL*fabs(x)+ZEPS;
	tol2 = 2.0*(tol1);
	if (fabs(x-middle) <= (tol2-0.5*(high-low))) {
	  *xmin= x;
	  ymin= fx;
	  return;
	}

	if (fabs(oldstep) > tol1) {
	  r=(x - w) * (fx - fv);
	  q=(x - v) * (fx - fw);
	  p=(x - v) * q + (w - x)* r;
	  q= 2.0 * (q - r);
	  if (q > 0.0) p = -p;
	  q = fabs(q);
	  steptemp = oldstep;
	  oldstep = step;
	  if ((fabs(p) >= fabs(0.5*q*steptemp)) || (p <= q*(low-x)) || (p >= q*(high-x))) {
	if (x >= middle)
	  oldstep = low - x;
	else
	  oldstep = high - x;
	step = CGOLD*oldstep;
	  } else {
	step = p/q;
	u = x + step;
	if ((u-low < tol2) || (high-u < tol2)) {
	  if ((middle - x) > 0.0)
		step = fabs(tol1);
	  else
		step = -fabs(tol1);
	}
	  }

	} else {
	  if (x >= middle)
	oldstep = low - x;
	  else
	oldstep = high - x;
	  step = CGOLD*oldstep;
	}

	if (fabs(step) >= tol1)
	  u =  x + step;
	else {
	  if (step > 0.0)
	u = x + fabs(tol1);
	  else
	u = x - fabs(tol1);
	}

	switch (type){
	case linearFit:
	case exponentialFit:
	  fu = chisq2(u);
	  break;
	case quadraticFit:
	  fu = chisq2_quad(u);
	  break;
	case quadErrorsCmin:
	  fu = chisq2_quad_c_error(u);
	  break;
	case quadErrorsBmin:
	  fu = chisq2_quad_b_error(u);
	  break;
	case quadErrorsAmin:
	  fu = chisq2_quad_a_error(u);
	  break;
	}

	if (fu <= fx) {
	  if (u >= x)
	low=x;
	  else
	high=x;
	  v = w;
	  w = x;
	  x = u;
	  fv = fw;
	  fw = fx;
	  fx = fu;
	} else {
	  if (u < x)
	low = u;
	  else
	high = u;
	  if ((fu <= fw) || (w == x)) {
	v = w;
	w = u;
	fv = fw;
	fw = fu;
	  } else {
	if ((fu <= fv) || (v == x) || (v == w)) {
	  v = u;
	  fv = fu;
	}
	  }
	}

	iter++;
  }

  if (iter > ITMAX) {
	*xmin = x;
	ymin = fx;
  }
}

double variance(double data[]){
	unsigned long j, n;
	double s, ep;
	double ave, var;

	n=ROWS;

	for (ave=0.0,j=1;j<=n;j++) ave += data[j];
	ave /=n;
	var=ep=0.0;
	for (j=1;j<=n;j++){
	s=data[j]-ave;
	ep+=s;
	var+=s*s;
	}
	var=(var-ep*ep/n)/(n-1);
	return var;
}

void linlsq2(int rows,double *x,double *y,double *sx, double *sy,double *a,double *aerr,double *b,double *berr,double *c) {

	double resize, *syNew, factor=0.01, small=TOLL, bmin, bmax;
	double cbmax, cbmin, btemp, atemp, ctemp, aetemp, betemp, W;
	int i, iter;

	globalType = linearFit;

	X=x;Y=y;SX=sx;SY=sy;ROWS=rows;

	resize = variance(y)/variance(x);
	printf("%f\n",resize);

	syNew = (double *)malloc(rows*sizeof(double));
	for (i=0;i<rows;i++) {
	syNew[i] = sqrt(pow(sy[i],2) + resize*resize*pow(sx[i],2));
	}

	linlsq1(rows,x,y,syNew,&atemp,&aetemp,&btemp,&betemp,&ctemp);
printf("%f %f %f\n",atemp,btemp,ctemp);
	iter=1;
	if (fabs(btemp)<small) btemp = small;

	bmax = btemp*(1+factor);
	bmin = btemp*(1-factor);

/* test */
	cbmax = chisq2(bmax);
	cbmin = chisq2(bmin);

	while (iter < 100) {
	cbmax = chisq2(bmax);
	cbmin = chisq2(bmin);
	if ((cbmin > ctemp) && (cbmax > ctemp))
		break;
	else {
		if (cbmin <= cbmax) {
		btemp = bmin;
		bmin = btemp*(1-factor);
		} else {
		btemp = bmax;
		bmax = btemp*(1+factor);
		}
	}
	if (fabs(btemp) < small)
		btemp = small;
	factor = factor*1.2;
	}

	paramin(bmin, btemp, bmax, &B, linearFit);
	*c = chisq2(B);
	realC = C;

	*a=A;
	*b=B;

	Berr = root(B*1e-8);
	*berr = Berr;

	atemp = *a;
	ctemp = chisq2(*b+Berr);

	W=0;
	for (i=0;i<ROWS;i++) W+=1/(pow(SX[i],2)*(*b+Berr)*(*b+Berr) + pow(SY[i],2));

	Aerr = sqrt(pow((A-atemp),2) +1/W);
	*aerr = Aerr;
}

double root(double xtemp){
	double small=TOLL, factor=0.1, xmin, xmax, ypos=0, yneg=0;
	double x, x1, x2, x3, y1, y2, y3, middle, tol1, p, q, r, s;
	double min1, min2, step1, step2, ymin, ymax, xpos, xneg, xtemp2;
	int iter;

	iter = 1;

	if (fabs(xtemp) < small) {
	xtemp = small;
	xmin = small;
	xmax = small;
	} else {
	xmin = xtemp;
	xmax = xtemp;
	}

	while (iter < 100) {

	xmin = xmin - xtemp*factor;
	xmax = xmax + xtemp*factor;

	ymin = chi2err(xmin);

	if(ymin < 0 && (ymin > yneg || yneg==0)) {
		yneg = ymin;
		xneg = xmin;
	} else {
		if(ymin > 0 && (ymin < ypos || ypos==0)) {
		ypos = ymin;
		xpos = xmin;
		}
	}
	ymax = chi2err(xmax);
	if(ymax < 0 && (ymax > yneg || yneg==0)) {
		yneg = ymax;
		xneg = xmax;
	} else {
		if(ymax > 0 && (ymax < ypos || ypos==0)) {
		ypos = ymax;
		xpos = xmax;
		}
	}
	if (ypos > 0 && yneg < 0)
		break;
	factor = factor*(1.2);
	iter++;
	}

	x1 = xpos;
	x2 = xneg;
	x3 = xneg;
	y1 = ypos;
	y2 = yneg;
	y3 = yneg;
	iter = 1;

	while (iter < 100) {

	if ((y2 > 0 && y3 > 0) || (y2 < 0 && y3 < 0)) {
		x3 = x1;
		y3 = y1;
		step1 = x2 - x3;
		step2 = x2 - x3;
	}
	if (fabs(y2) > fabs(y3)) {
		x1 = x2;
		x2 = x3;
		x3 = x1;
		y1 = y2;
		y2 = y3;
		y3 = y1;
	}
	tol1 = 2*fabs(x2)*(3e-8)+0.5*TOLL;
	middle=0.5*(x3-x2);
	if ((fabs(middle) <= tol1) || (x2 == 0.0)) {
		x = x2;
		break;
	}



	if ((fabs(step1) >= tol1) && (fabs(x1) > fabs(x2))) {
		s = y2/y1;
		if(x1 == x3) {
		p = 2*middle*s;
		q = 1 - s;
		} else {
		q = y1/y3;
		r = y2/y3;
		p = s*(2*middle*q*(q-r)-(x2-x1)*(r-1));
		q = (q-1)*(r-1)*(s-1);
		}
		if (p > 0.0)
		q = -q;
		p = fabs(p);
		min1 = 3*middle*q - fabs(tol1*q);
		min2 = fabs(step1*q);
		if (min1 <min2)
		min2 = min1;
		if (2*p < min2) {
		step1 = step2;
		step2 = p/q;
		} else {
		step2 = middle;
		step1 = step2;
		}
	} else {
		step2 = middle;
		step1 = step2;
	}
	x1 = x2;
	y1 = y2;

	if (fabs(step2) > tol1)
		x2 = x2 + step2;
	else {
		if (middle > 0)
		x2 = x2 + fabs(tol1);
		else
		x2 = x2 - fabs(tol1);
	}

	y2 = chi2err(x2);
	iter = iter + 1;
	}
	if (iter > 100)
	x = x2;

	return x;
}

double chi2err(double btemp){
	double cnew, cdif, globalB;


	globalB = B+btemp;

	switch (globalType){
	case linearFit:
	case exponentialFit:
	cnew = chisq2(globalB);
	break;
	case quadraticFit:
	cnew = chisq2_quad(globalB);
	break;
	}

	cdif = 1 - (cnew - realC);

	return cdif;
}

void quad_min_fit(int rows,double *x,double *y,double *sx, double *sy,double *a,double *aerr,double *b,double *berr,double *c, double *std::cerr, double *chi2) {

	double resize, *syNew, factor=0.01, small=TOLL, bmin, bmax;
	double cbmax, cbmin, btemp, atemp, ctemp, aetemp, betemp, W;
	double chitemp;
	int i, iter;

	globalType = quadraticFit;

	X=x;Y=y;SX=sx;SY=sy;ROWS=rows;
	atemp=*a;btemp=*b;ctemp=*c;
	chitemp=*chi2;

	A = atemp; B=btemp; C=ctemp;


	for (i=0;i<100;i++){
	iter = 1;

	chi2Type=C_only;

	bmax = ctemp*(1+factor);
	bmin = ctemp*(1-factor);

	cbmax = chisq2_quad(bmax);
	cbmin = chisq2_quad(bmin);

	while (iter < 100) {
	cbmax = chisq2_quad(bmax);
	cbmin = chisq2_quad(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
		break;
	else {
		if (cbmin <= cbmax) {
		ctemp = bmin;
		bmin = ctemp*(1-factor);
		} else {
		ctemp = bmax;
		bmax = ctemp*(1+factor);
		}
	}
	if (fabs(ctemp) < small)
		ctemp = small;
	factor = factor*1.2;
	}

	paramin(bmin, ctemp, bmax, &C, quadraticFit);
	chitemp = chisq2_quad(C);

	iter = 1;

	chi2Type=A_and_B;

	bmax = btemp*(1+factor);
	bmin = btemp*(1-factor);

	cbmax = chisq2_quad(bmax);
	cbmin = chisq2_quad(bmin);

	while (iter < 100) {
	cbmax = chisq2_quad(bmax);
	cbmin = chisq2_quad(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
		break;
	else {
		if (cbmin <= cbmax) {
		btemp = bmin;
		bmin = btemp*(1-factor);
		} else {
		btemp = bmax;
		bmax = btemp*(1+factor);
		}
	}
	if (fabs(btemp) < small)
		btemp = small;
	factor = factor*1.2;
	}

	paramin(bmin, btemp, bmax, &B, quadraticFit);
	chitemp = chisq2_quad(B);
	ctemp = C;btemp = B;

	}
	*chi2 = chisq2_quad(B);
	*c = C;

	*a=A;
	*b=B;
	realChi2 = *chi2;


/*    Berr = root(B*1e-8);
	*berr = Berr;

	atemp = *a;
	ctemp = chisq2_quad(*b+Berr);

	W=0;
	for (i=0;i<ROWS;i++) W+=1/(pow(SX[i],2)*(*b+Berr)*(*b+Berr) + pow(SY[i],2));

	Aerr = sqrt(pow((A-atemp),2) +1/W);*/

	*aerr = calcAError();
	*berr = calcBError();
	*std::cerr = calcCError();
}

double chisq2_quad(double Bt) {
	double *err, temp1, temp2, *W;
	int i;

	if (chi2Type == C_only) {
	err = (double *)malloc(ROWS*sizeof(double));
	for (i=0;i<ROWS;i++) err[i]=pow(SX[i]*(2*Bt*X[i] + B),2) + pow(SY[i],2);

	Chi2 = 0;
	for (i=0;i<ROWS;i++) Chi2 += (pow(Y[i] - A - B*X[i] - Bt*X[i]*X[i],2)/err[i]);
	free(err);
	return Chi2;

	} else {

	err = (double *)malloc(ROWS*sizeof(double));
	for (i=0;i<ROWS;i++) err[i]=pow(SX[i]*(Bt+2*C*X[i]),2) + pow(SY[i],2);

	W = (double *)malloc(ROWS*sizeof(double));
	for (i=0;i<ROWS;i++) W[i]=1/err[i];

	temp1 = 0;
	for (i=0;i<ROWS;i++) temp1 += W[i] * (Y[i]-Bt*X[i]-C*X[i]*X[i]);

	temp2 = 0;
	for (i=0;i<ROWS;i++) temp2 += W[i];

	A = temp1/temp2;

	Chi2 = 0;
	for (i=0;i<ROWS;i++) Chi2 += (pow(Y[i] - A - Bt*X[i]-C*X[i]*X[i],2)/err[i]);
	free(err);
	free(W);
	return Chi2;
	}
}

double chisq2_quad_c_error(double Bt) {
  double *err, temp1, temp2, *W;
  int i;

  err = (double *)malloc(ROWS*sizeof(double));
  for (i=0;i<ROWS;i++) err[i]=pow(SX[i]*(2*Bt*X[i] + B),2) + pow(SY[i],2);

  Chi2 = 0;
  for (i=0;i<ROWS;i++) Chi2 += (pow(Y[i] - A - B*X[i] - Bt*X[i]*X[i],2)/err[i]);
  free(err);
  return Chi2;

}

double chisq2_quad_b_error(double Bt) {
  double *err, temp1, temp2;
  int i;

  err = (double *)malloc(ROWS*sizeof(double));
  for (i=0;i<ROWS;i++) err[i]=pow(SX[i]*(Bt+2*C*X[i]),2) + pow(SY[i],2);

  Chi2 = 0;
  for (i=0;i<ROWS;i++) Chi2 += (pow(Y[i] - A - Bt*X[i]-C*X[i]*X[i],2)/err[i]);
  free(err);
  return Chi2;

}

double chisq2_quad_a_error(double Bt) {
  double *err, temp1, temp2, *W;
  int i;

  err = (double *)malloc(ROWS*sizeof(double));
  for (i=0;i<ROWS;i++) err[i]=pow(SX[i]*(2*C*X[i] + B),2) + pow(SY[i],2);

  Chi2 = 0;
  for (i=0;i<ROWS;i++) Chi2+=(pow(Y[i] - Bt - B*X[i] - C*X[i]*X[i],2)/err[i]);
  free(err);
  return Chi2;

}

double calcAError() {
  double aT, bT, cT, dA, chiError, chiLoP, chiHiP, chiLoN, chiHiN;
  int i = 0, scanDirection = MOVE_AWAY, iter, j;
  double resize, factor=0.01, small=TOLL, bmin, bmax;
  double cbmax, cbmin, btemp, atemp, ctemp, aetemp, betemp;
  double chitemp, aerr;

  cT = C;
  bT = B;
  aT = A;
  atemp = A;
  btemp = B;
  ctemp = C;

  dA = A*(-0.0005);

  chiError = realChi2;

  chiLoP = (chiError +1)*0.9995;
  chiHiP = (chiError +1)*1.0005;
  chiLoN = (chiError -1)*0.9995;
  chiHiN = (chiError -1)*1.0005;



  A += dA;

  while (i<50) {
	for (j=0;j<10;j++) {
	  iter = 1;

	  btemp = B;
	  ctemp = C;

	  bmax = ctemp*(1+factor);
	  bmin = ctemp*(1-factor);

	  cbmax = chisq2_quad_c_error(bmax);
	  cbmin = chisq2_quad_c_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_c_error(bmax);
	cbmin = chisq2_quad_c_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		ctemp = bmin;
		bmin = ctemp*(1-factor);
	  } else {
		ctemp = bmax;
		bmax = ctemp*(1+factor);
	  }
	}
	if (fabs(ctemp) < small)
	  ctemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, ctemp, bmax, &C, quadErrorsCmin);
	  chitemp = chisq2_quad_c_error(C);

	  iter = 1;

	  bmax = btemp*(1+factor);
	  bmin = btemp*(1-factor);

	  cbmax = chisq2_quad_b_error(bmax);
	  cbmin = chisq2_quad_b_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_b_error(bmax);
	cbmin = chisq2_quad_b_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		btemp = bmin;
		bmin = btemp*(1-factor);
	  } else {
		btemp = bmax;
		bmax = btemp*(1+factor);
	  }
	}
	if (fabs(btemp) < small)
	  btemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, btemp, bmax, &B, quadErrorsBmin);
	  chitemp = chisq2_quad_b_error(B);
	}
	chiError = chitemp;

	if (((chiError < chiHiN) && (chiError > chiLoN)) ||
	((chiError < chiHiP) && (chiError > chiLoP))) {
	  aerr = fabs(aT - A);
	  A = aT;
	  B = bT;
	  C = cT;
	  return aerr;
	}

	switch (scanDirection) {
	case MOVE_AWAY:
	  if ((chiError > chiHiP) || (chiError < chiLoN)) {
	dA = dA * (-0.25);
	scanDirection = MOVE_BACK;
	  }
	  break;
	case MOVE_BACK:
	  if ((chiError > chiHiN) && (chiError < chiLoP)) {
	dA = dA * (-0.25);
	scanDirection = MOVE_AWAY;
	  }
	  break;
	}
	A += dA;
	i++;
  }
  aerr = fabs(aT - A);
  A = aT;
  B = bT;
  C = cT;
  return aerr;
}

double calcBError() {
  double aT, bT, cT, dB, chiError, chiLoP, chiHiP, chiLoN, chiHiN;
  int i = 0, scanDirection = MOVE_AWAY, iter, j;
  double resize, factor=0.01, small=TOLL, bmin, bmax;
  double cbmax, cbmin, btemp, atemp, ctemp, aetemp, betemp;
  double chitemp, berr;

  cT = C;
  bT = B;
  aT = A;
  atemp = A;
  btemp = B;
  ctemp = C;

  dB = B*(-0.0005);

  chiError = realChi2;

  chiLoP = (chiError +1)*0.9995;
  chiHiP = (chiError +1)*1.0005;
  chiLoN = (chiError -1)*0.9995;
  chiHiN = (chiError -1)*1.0005;



  B += dB;

  while (i<50) {
	for (j=0;j<10;j++) {
	  iter = 1;

	  atemp = A;
	  ctemp = C;

	  bmax = ctemp*(1+factor);
	  bmin = ctemp*(1-factor);

	  cbmax = chisq2_quad_c_error(bmax);
	  cbmin = chisq2_quad_c_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_c_error(bmax);
	cbmin = chisq2_quad_c_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		ctemp = bmin;
		bmin = ctemp*(1-factor);
	  } else {
		ctemp = bmax;
		bmax = ctemp*(1+factor);
	  }
	}
	if (fabs(ctemp) < small)
	  ctemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, ctemp, bmax, &C, quadErrorsCmin);
	  chitemp = chisq2_quad_c_error(C);

	  iter = 1;

	  bmax = atemp*(1+factor);
	  bmin = atemp*(1-factor);

	  cbmax = chisq2_quad_a_error(bmax);
	  cbmin = chisq2_quad_a_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_a_error(bmax);
	cbmin = chisq2_quad_a_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		atemp = bmin;
		bmin = atemp*(1-factor);
	  } else {
		atemp = bmax;
		bmax = atemp*(1+factor);
	  }
	}
	if (fabs(atemp) < small)
	  atemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, atemp, bmax, &A, quadErrorsAmin);
	  chitemp = chisq2_quad_a_error(A);
	}
	chiError = chitemp;

	if (((chiError < chiHiN) && (chiError > chiLoN)) ||
	((chiError < chiHiP) && (chiError > chiLoP))) {
	  berr = fabs(bT - B);
	  A = aT;
	  B = bT;
	  C = cT;
	  return berr;
	}

	switch (scanDirection) {
	case MOVE_AWAY:
	  if ((chiError > chiHiP) || (chiError < chiLoN)) {
	dB = dB * (-0.25);
	scanDirection = MOVE_BACK;
	  }
	  break;
	case MOVE_BACK:
	  if ((chiError > chiHiN) && (chiError < chiLoP)) {
	dB = dB * (-0.25);
	scanDirection = MOVE_AWAY;
	  }
	  break;
	}
	B += dB;
	i++;
  }
  berr = fabs(bT - B);
  A = aT;
  B = bT;
  C = cT;
  return berr;
}

double calcCError() {
  double aT, bT, cT, dC, chiError, chiLoP, chiHiP, chiLoN, chiHiN;
  int i = 0, scanDirection = MOVE_AWAY, iter, j;
  double resize, factor=0.01, small=TOLL, bmin, bmax;
  double cbmax, cbmin, btemp, atemp, ctemp, aetemp, betemp;
  double chitemp, std::cerr;

  cT = C;
  bT = B;
  aT = A;
  atemp = A;
  btemp = B;
  ctemp = C;

  dC = C*(-0.0005);

  chiError = realChi2;

  chiLoP = (chiError +1)*0.9995;
  chiHiP = (chiError +1)*1.0005;
  chiLoN = (chiError -1)*0.9995;
  chiHiN = (chiError -1)*1.0005;



  C += dC;

  while (i<50) {
	for (j=0;j<10;j++) {
	  iter = 1;

	  btemp = B;
	  atemp = A;

	  bmax = atemp*(1+factor);
	  bmin = atemp*(1-factor);

	  cbmax = chisq2_quad_a_error(bmax);
	  cbmin = chisq2_quad_a_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_a_error(bmax);
	cbmin = chisq2_quad_a_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		atemp = bmin;
		bmin = atemp*(1-factor);
	  } else {
		atemp = bmax;
		bmax = atemp*(1+factor);
	  }
	}
	if (fabs(atemp) < small)
	  atemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, atemp, bmax, &A, quadErrorsAmin);
	  chitemp = chisq2_quad_a_error(C);

	  iter = 1;

	  bmax = btemp*(1+factor);
	  bmin = btemp*(1-factor);

	  cbmax = chisq2_quad_b_error(bmax);
	  cbmin = chisq2_quad_b_error(bmin);

	  while (iter < 100) {
	cbmax = chisq2_quad_b_error(bmax);
	cbmin = chisq2_quad_b_error(bmin);
	if ((cbmin > chitemp) && (cbmax > chitemp))
	  break;
	else {
	  if (cbmin <= cbmax) {
		btemp = bmin;
		bmin = btemp*(1-factor);
	  } else {
		btemp = bmax;
		bmax = btemp*(1+factor);
	  }
	}
	if (fabs(btemp) < small)
	  btemp = small;
	factor = factor*1.2;
	iter++;
	  }

	  paramin(bmin, btemp, bmax, &B, quadErrorsBmin);
	  chitemp = chisq2_quad_b_error(B);
	}
	chiError = chitemp;

	if (((chiError < chiHiN) && (chiError > chiLoN)) ||
	((chiError < chiHiP) && (chiError > chiLoP))) {
	  std::cerr = fabs(cT - C);
	  A = aT;
	  B = bT;
	  C = cT;
	  return std::cerr;
	}

	switch (scanDirection) {
	case MOVE_AWAY:
	  if ((chiError > chiHiP) || (chiError < chiLoN)) {
	dC = dC * (-0.25);
	scanDirection = MOVE_BACK;
	  }
	  break;
	case MOVE_BACK:
	  if ((chiError > chiHiN) && (chiError < chiLoP)) {
	dC = dC * (-0.25);
	scanDirection = MOVE_AWAY;
	  }
	  break;
	}
	C += dC;
	i++;
  }
  std::cerr = fabs(cT - C);
  A = aT;
  B = bT;
  C = cT;
  return std::cerr;
}
