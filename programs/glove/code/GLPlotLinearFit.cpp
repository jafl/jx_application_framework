/*********************************************************************************
 GLPlotLinearFit.cpp

	GLPlotLinearFit class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotLinearFit.h"

#include "J2DPlotWidget.h"
#include "J2DPlotData.h"

#include <JArray.h>
#include <JString.h>
#include <jMath.h>
#include <jAssert.h>

const JSize 	ITMAX  = 100;
const JFloat 	CGOLD  = 0.3819660;
const JFloat 	ZEPS   = 1.0e-10;
const JFloat 	TOLL   = 1.0e-10;

/*********************************************************************************
 Constructor


 ********************************************************************************/

GLPlotLinearFit::GLPlotLinearFit
	(
	J2DPlotWidget* 	plot,
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax,
	const bool 	xlog,
	const bool 	ylog
	)
	:
	GLPlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = false;
	JPlotLinearFitX(plot, fitData, xlog, ylog);
}

GLPlotLinearFit::GLPlotLinearFit
	(
	J2DPlotWidget* plot,
	JPlotDataBase* fitData,
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax,
	const bool xlog,
	const bool ylog
	)
	:
	GLPlotFitFunction(plot, fitData, xmin, xmax)
{
	if (xmax > xmin)
		{
		itsRangeXMax = xmax;
		itsRangeXMin = xmin;
		}
	else
		{
		itsRangeXMax = xmin;
		itsRangeXMin = xmax;
		}
	if (ymax > ymin)
		{
		itsRangeYMax = ymax;
		itsRangeYMin = ymin;
		}
	else
		{
		itsRangeYMax = ymin;
		itsRangeYMin = ymax;
		}
	itsUsingRange = true;
	JPlotLinearFitX(plot, fitData, xlog, ylog);
}

void
GLPlotLinearFit::JPlotLinearFitX
	(
	J2DPlotWidget* plot,
	JPlotDataBase* fitData,
	const bool xlog,
	const bool ylog
	)
{
	SetHasParameterErrors(true);
	SetParameterCount(2);
	SetHasGoodnessOfFit(true);
	itsXIsLog = xlog;
	itsYIsLog = ylog;
	if (!xlog && !ylog)
		{
		itsFunctionName.Set("y = a+bx");
		}
	else if (!xlog && ylog)
		{
		itsFunctionName.Set("y = a Exp(bx)");
		}
	itsRealCount = 0;
	GenerateFit();
	GenerateDiffData();
	if (itsYIsLog && !itsXIsLog)
		{
		AdjustDiffData();
		}
}

/*********************************************************************************
 Destructor


 ********************************************************************************/

GLPlotLinearFit::~GLPlotLinearFit()
{
}

/*********************************************************************************
 GetElement


 ********************************************************************************/
/*
void
GLPlotLinearFit::GetElement
	(
	const JIndex index,
	J2DDataPoint* data
	)
	const
{
	JFloat a;
	JFloat b;
	GetParameter(1, &a);
	GetParameter(2, &b);
	JFloat range = itsCurrentXMax - itsCurrentXMin;
	JFloat x;
	if (itsXIsLog)
		{

		}
	else
		{
		x = itsCurrentXMin + (index-1)*range/itsCurrentStepCount;
		}
	JFloat y;
	if (itsYIsLog)
		{
		y = a * exp(b*x);
		}
	else
		{
		y = a + b*x;
		}
	data->x = x;
	data->y = y;
}*/

/*********************************************************************************
 GetYValue


 ********************************************************************************/

bool
GLPlotLinearFit::GetYValue
	(
	const JFloat 	x,
	JFloat* 		y
	)
	const
{
	JFloat a;
	JFloat b;
	GetParameter(1, &a);
	GetParameter(2, &b);

	if (itsYIsLog)
		{
		*y = a * exp(b*x);
		}
	else
		{
		*y = a + b*x;
		}

	return true;
}


/*********************************************************************************
 GetYRange


 ********************************************************************************/

bool
GLPlotLinearFit::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool	xLinear,
	JFloat*			yMin,
	JFloat*			yMax
	)
	const
{
	JFloat y1, y2;
	GetYValue(xMin, &y1);
	GetYValue(xMax, &y2);

	*yMin = JMin(y1, y2);
	*yMax = JMax(y1, y2);
	return true;
}

/*********************************************************************************
 UpdateFunction


 ********************************************************************************/
/*
void
GLPlotLinearFit::UpdateFunction
	(
	const JFloat xmin,
	const JFloat xmax,
	const JSize steps
	)
{
	itsCurrentXMin = xmin;
	itsCurrentXMax = xmax;
	itsCurrentStepCount = steps;
	SetElementCount(steps+1);
}*/

/*********************************************************************************
 GetParameterName


 ********************************************************************************/

bool
GLPlotLinearFit::GetParameterName
	(
	const JIndex index,
	JString* name
	)
	const
{
	if ((index > 2) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		*name = "a";
		}
	if (index == 2)
		{
		*name = "b";
		}
	return true;
}

/*********************************************************************************
 GetParameter


 ********************************************************************************/

bool
GLPlotLinearFit::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	if ((index > 2) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		if (!itsXIsLog)
			{
			if (!itsYIsLog)
				{
				*value = itsAParameter;
				}
			else
				{
				*value = exp(itsAParameter);
				}
			}
		}
	if (index == 2)
		{
		*value = itsBParameter;
		}
	return true;
}

/*********************************************************************************
 GetParameterError


 ********************************************************************************/

bool
GLPlotLinearFit::GetParameterError
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (!data->HasXErrors() && !data->HasYErrors())
		{
		return false;
		}
	if (index == 1)
		{
		if (!itsXIsLog)
			{
			if (!itsYIsLog)
				{
				*value = itsAErrParameter;
				}
			else
				{
				*value = exp(itsAParameter) - exp(itsAParameter - itsAErrParameter);
				}
			}
		}
	else
		{
		*value = itsBErrParameter;
		}
	return true;
}

/*********************************************************************************
 GetGoodnessOfFitName


 ********************************************************************************/

bool
GLPlotLinearFit::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors() || data->HasYErrors())
		{
		*name = "Chi^2/(N-2)";
		}
	else
		{
		*name = "Std dev";
		}
	return true;
}

/*********************************************************************************
 GetGoodnessOfFit


 ********************************************************************************/

bool
GLPlotLinearFit::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors() || data->HasYErrors())
		{
		*value = itsChi2/(itsRealCount - 2);
		}
	else
		{
		*value = GetStdDev();
		}

	return true;
}

/*********************************************************************************
 GetFunctionString


 ********************************************************************************/

JString
GLPlotLinearFit::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString


 ********************************************************************************/

JString
GLPlotLinearFit::GetFitFunctionString()
	const
{
	return itsFunctionName;
}


/*********************************************************************************
 GenerateFit


 ********************************************************************************/

void
GLPlotLinearFit::GenerateFit()
{
	LinearLSQ1();
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors())
		{
		LinearLSQ2();
		}
}

/*********************************************************************************
 LinearLSQ1


 ********************************************************************************/

void
GLPlotLinearFit::LinearLSQ1()
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	const JSize count = data->GetElementCount();
	JArray<JFloat> weight;
	JArray<JFloat> sigma;
	JSize i;

	JFloat vx, vy;
	Variance(&vx,&vy);
	JFloat resize = vy/vx;
	JFloat num = 0;
	JFloat avgx = 0;

	for (i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat sy = point.yerr;
			JFloat sx = point.xerr;
			if (sy == 0)
				{
				sy = 1;
				}
			JFloat s = 0;
			if (!itsXIsLog)
				{
				if (!itsYIsLog)
					{
					s = sqrt(sy * sy + resize * resize * sx * sx);
					}
				else
					{
					s = sqrt(sy * sy + resize * resize * point.y * point.y * sx * sx);
					}
				}
			else
				{
				// do for power law;
				}
			sigma.AppendElement(s);
			JFloat w = 1/(s*s);
			weight.AppendElement(w);
			num += w;
			avgx += w * point.x;
			itsRealCount++;
			}
		}

	avgx /= num;

	JArray<JFloat> t;
	JFloat stt = 0;
	JFloat b = 0;
	JSize counter = 1;
	for (i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat tTemp = (point.x - avgx)/(sigma.GetElement(counter));
			t.AppendElement(tTemp);
			stt += 	tTemp * tTemp;
			b += tTemp * point.y / sigma.GetElement(counter);
			counter++;
			}
		}
	b /= stt;

	JFloat a = 0;
	JFloat aerr = 0;
	counter = 1;
	for (i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat w = weight.GetElement(counter);
			a += w * (point.y - b * point.x);
			aerr += w * w * point.x * point.x;
			counter++;
			}
		}
	a /= num;
	aerr /= (num * stt);
	aerr += 1;
	aerr /= num;
	aerr = sqrt(aerr);

	JFloat berr = sqrt(1.0/stt);

	JFloat c = 0;
	bool sytest = true;
	counter = 1;
	for (i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat temp = (point.y - a - b * point.x)/(sigma.GetElement(counter));
			c += temp * temp;
			if (sigma.GetElement(counter) != 1)
				{
				sytest = false;
				}
			counter++;
			}
		}

	if (sytest)
		{
		JFloat sig = sqrt(c/count);
		aerr = aerr * sig;
		berr = berr * sig;
		}

	itsAParameter = a;
	itsAErrParameter = aerr;
	itsBParameter = b;
	itsBErrParameter = berr;
	itsChi2 = c;
}

/*********************************************************************************
 variance


 ********************************************************************************/

void
GLPlotLinearFit::Variance
	(
	JFloat* vx,
	JFloat* vy
	)
{
	JSize j;
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	const JSize count = data->GetElementCount();
	JFloat sy, sx;
	JFloat epy = 0, epx = 0;
	JFloat avey = 0, avex = 0;
	JFloat vary = 0, varx = 0;

	for (j = 1; j <= count; j++)
		{
		if (GetDataElement(j, &point))
			{
			avey += point.y;
			avex += point.x;
			}
		}
	avey /= count;
	avex /= count;

	for (j = 1; j <= count; j++)
		{
		if (GetDataElement(j, &point))
			{
			sy = point.y - avey;
			sx = point.x - avex;
			epy += sy;
			epx += sx;
			vary += sy * sy;
			varx += sx * sx;
			}
		}
	*vy = (vary - epy * epy/count)/(count-1);
	*vx = (varx - epx * epx/count)/(count-1);
}

/*********************************************************************************
 LinearLSQ2


 ********************************************************************************/

void
GLPlotLinearFit::LinearLSQ2()
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	const JSize count = data->GetElementCount();

	JFloat factor=0.01;
	JFloat small=TOLL;
	JSize i;

	JFloat btemp = itsBParameter;
	JFloat ctemp = itsChi2;
	JSize iter = 1;

	for (i = 1; i <= 3; i++)
		{
		btemp = itsBParameter;
		ctemp = itsChi2;
		iter = 1;

		 if (fabs(btemp)<small)
			{
			btemp = small;
			}

		JFloat bmax = btemp*(1+factor);
		JFloat bmin = btemp*(1-factor);

		JFloat cbmax = ChiSqr(bmax);
		JFloat cbmin = ChiSqr(bmin);

		while (iter < 100)
			{
			cbmax = ChiSqr(bmax);
			cbmin = ChiSqr(bmin);
			if ((cbmin > ctemp) && (cbmax > ctemp))
				{
				break;
				}
			else
				{
				if (cbmin <= cbmax)
					{
					btemp = bmin;
					bmin = btemp*(1-factor);
					}
				else
					{
					btemp = bmax;
					bmax = btemp*(1+factor);
					}
				}
			if (fabs(btemp) < small)
				{
				btemp = small;
				}
			factor = factor*1.2;
			iter++;
			}

		JFloat Bt;
		Paramin(bmin, btemp, bmax, &Bt);
		itsBParameter = Bt;
		itsChi2 = ChiSqr(Bt);
	}
	itsAParameter = itsAParameterT;

	itsBErrParameter = Root(itsBParameter*1e-8);

	JFloat W=0;
	for (i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat sx = point.xerr;
			JFloat sy = point.yerr;
			if (sy == 0)
				{
				sy = 1;
				}
			W += 1/(sx * sx * (itsBParameter + itsBErrParameter) *
					(itsBParameter + itsBErrParameter) + sy * sy);
			}
		}
	JFloat val = itsAParameterT-itsAParameter;
	itsAErrParameter = sqrt(val * val + 1/W);
}

/*********************************************************************************
 ChiSqr


 ********************************************************************************/

JFloat
GLPlotLinearFit::ChiSqr
	(
	JFloat Bt
	)
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	const JSize count = data->GetElementCount();
	JArray<JFloat> err;
	JArray<JFloat> W;
	JFloat temp1;
	JFloat temp2;
	JSize i;

	temp1 = 0;
	temp2 = 0;
	for ( i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat sy = point.yerr;
			if (sy == 0)
				{
				sy = 1;
				}
			JFloat e = point.xerr * point.xerr * Bt * Bt + sy * sy;
			err.AppendElement(e);
			JFloat w = 1/e;
			W.AppendElement(w);
			temp1 += w * (point.y - Bt*point.x);
			temp2 += w;
			}
		}

	itsAParameterT = temp1/temp2;

	JFloat c = 0;
	JSize counter = 1;
	for ( i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &point))
			{
			JFloat val = point.y - itsAParameterT - Bt*point.x;
			c += (val*val)/(err.GetElement(counter));
			counter++;
			}
		}

	return c;//sqrt(fabs(c));
}

/*********************************************************************************
 Paramin


 ********************************************************************************/

void
GLPlotLinearFit::Paramin
	(
	JFloat ax,
	JFloat bx,
	JFloat cx,
	JFloat* xmin
	)
{
	JFloat oldstep, x, w, v, fx, fw, fv, middle, tol2, ymin, r, q;
	JFloat p, steptemp, tol1, step, low, high, u, fu;
	JSize iter;

	oldstep = 0.0;
	x = bx;
	w = bx;
	v = bx;


	fx = ChiSqr(bx);
	fw = fx;
	fw = fx;
	if (ax<cx)
		{
		low = ax;
		high = cx;
		}
	else
		{
		low = cx;
		high = ax;
		}
	iter = 1;
	while (iter<= ITMAX)
		{
		middle=0.5*(low+high);
		tol1=TOLL*fabs(x)+ZEPS;
		tol2 = 2.0*(tol1);
		if (fabs(x-middle) <= (tol2-0.5*(high-low)))
			{
			*xmin= x;
			ymin= fx;
			return;
			}
		if (fabs(oldstep) > tol1)
			{
			r=(x - w) * (fx - fv);
			q=(x - v) * (fx - fw);
			p=(x - v) * q + (w - x)* r;
			q= 2.0 * (q - r);
			if (q > 0.0)
				{
				p = -p;
				}
			q = fabs(q);
			steptemp = oldstep;
			oldstep = step;
			if ((fabs(p) >= fabs(0.5*q*steptemp)) || (p <= q*(low-x)) || (p >= q*(high-x)))
				{
				if (x >= middle)
					{
					oldstep = low - x;
					}
				else
					{
					oldstep = high - x;
					}
				step = CGOLD*oldstep;
				}
			else
				{
				step = p/q;
				u = x + step;
				if ((u-low < tol2) || (high-u < tol2))
					{
					if ((middle - x) > 0.0)
						{
						step = fabs(tol1);
						}
					else
						{
						step = -fabs(tol1);
						}
					}
				}
			}
		else
			{
			if (x >= middle)
				{
				oldstep = low - x;
				}
			else
				{
				oldstep = high - x;
				}
			step = CGOLD*oldstep;
			}

		if (fabs(step) >= tol1)
			{
			u = x + step;
			}
		else
			{
			if (step > 0.0)
				{
				u = x + fabs(tol1);
				}
			else
				{
				u = x - fabs(tol1);
				}
			}
		fu = ChiSqr(u);
		if (fu <= fx)
			{
			if (u >= x)
				{
				low=x;
				}
			else
				{
				high=x;
				}
			v = w;
			w = x;
			x = u;
			fv = fw;
			fw = fx;
			fx = fu;
			}
		else
			{
			if (u < x)
				{
				low = u;
				}
			else
				{
				high = u;
				}
			if ((fu <= fw) || (w == x))
				{
				v = w;
				w = u;
				fv = fw;
				fw = fu;
				}
			else
				{
				if ((fu <= fv) || (v == x) || (v == w))
					{
					v = u;
					fv = fu;
					}
				}
			}

		iter++;
		}

	*xmin = x;
}

/*********************************************************************************
 Root


 ********************************************************************************/

JFloat
GLPlotLinearFit::Root
	(
	JFloat xtemp
	)
{
	JFloat small=TOLL, factor=0.1, xmin, xmax, ypos=0, yneg=0;
	JFloat x, x1, x2, x3, y1, y2, y3, middle, tol1, p, q, r, s;
	JFloat min1, min2, step1, step2, ymin, ymax, xpos, xneg;
	int iter;

	iter = 1;

	if (fabs(xtemp) < small)
		{
		xtemp = small;
		xmin = small;
		xmax = small;
		}
	else
		{
		xmin = xtemp;
		xmax = xtemp;
	}

	while (iter < 100)
		{

		xmin = xmin - xtemp*factor;
		xmax = xmax + xtemp*factor;

		ymin = ChiSqrErr(xmin);

		if (ymin < 0 && (ymin > yneg || yneg==0))
			{
			yneg = ymin;
			xneg = xmin;
			}
		else
			{
			if(ymin > 0 && (ymin < ypos || ypos==0)) {
			ypos = ymin;
			xpos = xmin;
			}
		}
		ymax = ChiSqrErr(xmax);
		if(ymax < 0 && (ymax > yneg || yneg==0))
			{
			yneg = ymax;
			xneg = xmax;
			}
		else
			{
			if(ymax > 0 && (ymax < ypos || ypos==0))
				{
				ypos = ymax;
				xpos = xmax;
				}
			}
		if (ypos > 0 && yneg < 0)
			{
			break;
			}
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

	while (iter < 100)
		{

		if ((y2 > 0 && y3 > 0) || (y2 < 0 && y3 < 0))
			{
			x3 = x1;
			y3 = y1;
			step1 = x2 - x3;
			step2 = x2 - x3;
			}
		if (fabs(y2) > fabs(y3))
			{
			x1 = x2;
			x2 = x3;
			x3 = x1;
			y1 = y2;
			y2 = y3;
			y3 = y1;
			}
		tol1 = 2*fabs(x2)*(3e-8)+0.5*TOLL;
		middle=0.5*(x3-x2);
		if ((fabs(middle) <= tol1) || (x2 == 0.0))
			{
			x = x2;
			break;
			}

		if ((fabs(step1) >= tol1) && (fabs(x1) > fabs(x2)))
			{
			s = y2/y1;
			if(x1 == x3)
				{
				p = 2*middle*s;
				q = 1 - s;
				}
			else
				{
				q = y1/y3;
				r = y2/y3;
				p = s*(2*middle*q*(q-r)-(x2-x1)*(r-1));
				q = (q-1)*(r-1)*(s-1);
				}
			if (p > 0.0)
				{
				q = -q;
				}
			p = fabs(p);
			min1 = 3*middle*q - fabs(tol1*q);
			min2 = fabs(step1*q);
			if (min1 <min2)
			min2 = min1;
			if (2*p < min2)
				{
				step1 = step2;
				step2 = p/q;
				}
			else
				{
				step2 = middle;
				step1 = step2;
				}
			}
		else
			{
			step2 = middle;
			step1 = step2;
			}
		x1 = x2;
		y1 = y2;

		if (fabs(step2) > tol1)
			{
			x2 = x2 + step2;
			}
		else
			{
			if (middle > 0)
				{
				x2 = x2 + fabs(tol1);
				}
			else
				{
				x2 = x2 - fabs(tol1);
				}
			}

		y2 = ChiSqrErr(x2);
		iter = iter + 1;
		}
	if (iter > 100)
		{
		x = x2;
		}
	return x;
}

/*********************************************************************************
 ChiSqrErr


 ********************************************************************************/

JFloat
GLPlotLinearFit::ChiSqrErr
	(
	JFloat Bt
	)
{
	JFloat globalB = itsBParameter + Bt;
	JFloat cnew = ChiSqr(globalB);
	JFloat cdif = 1 - (cnew - itsChi2);
	return cdif;
}

/*********************************************************************************
 SetFunctionName


 ********************************************************************************/

void
GLPlotLinearFit::SetFunctionName
	(
	const JString& name
	)
{
	itsFunctionName = name;
}

/*********************************************************************************
 DataElementValid


 ********************************************************************************/

bool
GLPlotLinearFit::DataElementValid
	(
	const JIndex index
	)
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	data->GetElement(index, &point);

	if (itsYIsLog)
		{
		if (point.y <= 0)
			{
			return false;
			}
		}
	if (itsXIsLog)
		{
		if (point.x <= 0)
			{
			return false;
			}
		}
	if (itsUsingRange)
		{
		if ((point.x >= itsRangeXMin) &&
			(point.x <= itsRangeXMax) &&
			(point.y >= itsRangeYMin) &&
			(point.y <= itsRangeYMax))
			{
			return true;
			}
		else
			{
			return false;
			}
		}
	return true;
}

/******************************************************************************
 GetDataElement


 *****************************************************************************/

bool
GLPlotLinearFit::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	bool valid = DataElementValid(index);
	if (!valid)
		{
		return false;
		}
	const JPlotDataBase* data = GetDataToFit();
	data->GetElement(index, point);
	if (itsYIsLog)
		{
		JFloat y = point->y;
		point->y = log(y);
		const JPlotDataBase* data = GetData();
		if (data->HasYErrors())
			{
			JFloat yerr = point->yerr;
			point->yerr = fabs(log((y - yerr)/y));
			}
		}
	return true;
}

/*****************************************************************************
 AdjustDiffData


 *****************************************************************************/

void
GLPlotLinearFit::AdjustDiffData()
{
	JFloat B = itsBParameter;
	JPlotDataBase* pwd = GetDiffData();
	J2DDataPoint dataD;
	J2DDataPoint data;
	const JSize count = pwd->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		pwd->GetElement(i, &dataD);
		GetData()->GetElement(i, &data);
		JFloat yerr;
		if (GetData()->HasYErrors() || GetData()->HasXErrors())
			{
			yerr = sqrt(data.yerr*data.yerr + B*B*data.y*data.y*data.xerr*data.xerr);
			AdjustDiffDataValue(i, yerr);
			}
		}
}
