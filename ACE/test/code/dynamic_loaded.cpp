/******************************************************************************
 dynamic_loaded.cpp

	This code is dynamically loaded by dynamic_loader.cc.

	Written by Glenn Bach & John Lindal.

 ******************************************************************************/

extern "C"
{
	void	Set(const double v1, const double v2, const double v3);
	double 	Eval(const double x);
}

static double a, b, c;

void
Set
	(
	const double v1, 
	const double v2, 
	const double v3
	)
{
	a = v1;
	b = v2;
	c = v3;
}

double
Eval
	(
	const double x
	)
{
	return a + b * x + c * x * x;
}
