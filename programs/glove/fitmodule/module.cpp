#include <math.h>

extern "C"
{
	double			FunctionN(const double* parms, const double x);
	double			FunctionNPrimed(const double* parms, const double x);
	const char**	GetParms();
	unsigned long	GetParmCount();
	const char*		GetFunctionForm();
	const char*		GetFitName();
}

const char* parmnames[]	= {"a1", "a2", "a3", "a4", "a5"};
const char* kForm			= "a1 + a2*e^(a3*x) + a4*e^(a5*x)";
const char* kName			= "Two exponentials + C";

/******************************************************************************
 FunctionN

 ******************************************************************************/

double
FunctionN
	(
	const double* 	parms,
	const double 	x
	)
{
	return parms[0] + parms[1] * exp(parms[2] * x) + parms[3] * exp(parms[4] * x);
}

/******************************************************************************
 FunctionNPrimed

 ******************************************************************************/

double
FunctionNPrimed
	(
	const double* 	parms,
	const double 	x
	)
{
	return parms[1] * parms[2] * exp(parms[2] * x) + parms[3] * parms[4] * exp(parms[4] * x);
}

/******************************************************************************
 GetParms

 ******************************************************************************/

const char**
GetParms()
{
	return parmnames;
}

/******************************************************************************
 GetParmCount

 ******************************************************************************/

unsigned long
GetParmCount()
{
	return sizeof(parmnames)/sizeof(char*);
}

/******************************************************************************
 GetFunctionForm

 ******************************************************************************/

const char*
GetFunctionForm()
{
	return kForm;
}

/******************************************************************************
 GetFitName

 ******************************************************************************/

const char*
GetFitName()
{
	return kName;
}
