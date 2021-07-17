/*********************************************************************************
 GLPlotQuadFit.cpp

	GLPlotQuadFit class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotQuadFit.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"

#include "GLPlotFitQuad.h"
#include "GLPlotFitQuad2.h"

#include <JString.h>
#include <JArray.h>
#include <JMatrix.h>
#include <JVector.h>

#include <jMath.h>
#include <JMinMax.h>
#include <jAssert.h>

const JSize 	ITMAX	= 100;
const JFloat 	CGOLD	= 0.3819660;
const JFloat 	ZEPS	= 1.0e-12;
const JFloat 	TOLL	= 1.0e-10;
const JFloat 	GOLD 	= 1.618034;
const JFloat 	GLIMIT 	= 100.0;
const JFloat 	TINY 	= 1.0e-20;

enum
{
	MOVE_AWAY=1,
	MOVE_BACK
};

enum
{
	kChiCType = 1,
	kChiAType,
	kChiBType,
	kChiABType,
	kDefaultType
};

enum
{
	kAFixed = 1,
	kBFixed,
	kCFixed,
	kMinimizeAll
};

enum
{
	kAError = 1,
	kBError,
	kCError
};

enum
{
	k1DimType = 1,
};

/*********************************************************************************
 Constructor


 ********************************************************************************/

GLPlotQuadFit::GLPlotQuadFit
	(
	J2DPlotWidget* 	plot,
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = false;
	JPlotQuadFitX(plot, fitData);
	GLPlotFitQuad junk(plot, fitData, xMin, xMax);
//	GLPlotFitQuad2 junk2(plot, fitData, xMin, xMax);
}

GLPlotQuadFit::GLPlotQuadFit
	(
	J2DPlotWidget* plot,
	JPlotDataBase* fitData,
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax
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
	JPlotQuadFitX(plot, fitData);
}

void
GLPlotQuadFit::JPlotQuadFitX
	(
	J2DPlotWidget* plot,
	JPlotDataBase* fitData
	)
{
	itsP 	= nullptr;
	itsXi	= nullptr;

	SetHasParameterErrors(true);
	if (fitData->HasXErrors() || fitData->HasYErrors())
		{
		SetHasParameterErrors(true);
		}
	else
		{
		SetHasParameterErrors(false);
		}
	SetParameterCount(3);
	SetHasGoodnessOfFit(true);
	itsFunctionName.Set("y = a + bx + cx^2");

	itsRealCount = 0;

	itsRealData = jnew JArray<J2DDataPoint>;
	assert(itsRealData != nullptr);
	const JSize count = fitData->GetElementCount();
	for (JSize i=1; i<= count; i++)
		{
		J2DDataPoint point;
		if (GetDataElement(i, &point))
			{
			itsRealData->AppendElement(point);
			}
		}
	GenerateFit();
	GenerateDiffData();
	AdjustDiffData();
}


/*********************************************************************************
 Destructor


 ********************************************************************************/

GLPlotQuadFit::~GLPlotQuadFit()
{
}

/*********************************************************************************
 GetYValue


 ********************************************************************************/

bool
GLPlotQuadFit::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	JFloat a;
	JFloat b;
	JFloat c;
	GetParameter(1, &a);
	GetParameter(2, &b);
	GetParameter(3, &c);
	*y = a + b*x + c*x*x;
	return true;
}

/*********************************************************************************
 GetParameterName


 ********************************************************************************/

bool
GLPlotQuadFit::GetParameterName
	(
	const JIndex index,
	JString* name
	)
	const
{
	if ((index > 3) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		*name = "a";
		}
	else if (index == 2)
		{
		*name = "b";
		}
	else if (index == 3)
		{
		*name = "c";
		}
	return true;
}

/*********************************************************************************
 GetParameter


 ********************************************************************************/

bool
GLPlotQuadFit::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	if ((index > 3) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		*value = itsAParameter;
		}
	else if (index == 2)
		{
		*value = itsBParameter;
		}
	else if (index == 3)
		{
		*value = itsCParameter;
		}
	return true;
}

/*********************************************************************************
 GetParameterError


 ********************************************************************************/

bool
GLPlotQuadFit::GetParameterError
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
		*value = itsAErrParameter;
		}
	else if (index == 2)
		{
		*value = itsBErrParameter;
		}
	else if (index == 3)
		{
		*value = itsCErrParameter;
		}
	return true;
}

/*********************************************************************************
 GetGoodnessOfFitName


 ********************************************************************************/

bool
GLPlotQuadFit::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors() || data->HasYErrors())
		{
		*name = "Chi^2/(N-3)";
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
GLPlotQuadFit::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors() || data->HasYErrors())
		{
		*value = itsChi2/(itsRealCount - 3);
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
GLPlotQuadFit::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString


 ********************************************************************************/

JString
GLPlotQuadFit::GetFitFunctionString()
	const
{
	return itsFunctionName;
}


/*********************************************************************************
 GenerateFit


 ********************************************************************************/

void
GLPlotQuadFit::GenerateFit()
{
	QuadFirstPass();
	const JPlotDataBase* data = GetDataToFit();
	if (data->HasXErrors() || data->HasYErrors())
		{
		QuadMinFit();
		}
}

/*********************************************************************************
 QuadFirstPass


 ********************************************************************************/

void
GLPlotQuadFit::QuadFirstPass()
{
	JFloat Y, X, X2, YX, X3, YX2, X4, Sig;
	JFloat tempa, tempb, tempc, det;
	JSize i,j, k;
	JArray<JFloat> yAdjError;

	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	const JSize count = data->GetElementCount();
	JSize rcount = itsRealData->GetElementCount();
	for (i=1; i<= count; i++)
		{
		J2DDataPoint point;
		if (GetDataElement(i, &point))
			{
			JFloat newVal = 1;
			if (point.yerr != 0)
				{
				newVal = point.yerr;
				}
			yAdjError.AppendElement(newVal);
			}
		}

	JMatrix odata(rcount, 3, 1.0);
	JVector yData(rcount);
	for (i=1; i<= rcount; i++)
		{
		point = itsRealData->GetElement(i);
		odata.SetElement(i, 1, 1);
		odata.SetElement(i, 2, point.x);
		odata.SetElement(i, 3, point.x*point.x);
		yData.SetElement(i, point.y);
		}
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(3,1);
	JGaussianElimination(lData, rData, &parms);

	for (k=1; k<= 4; k++)
		{
		Y = 0;
		X = 0;
		X2 = 0;
		YX = 0;
		X3 = 0;
		YX2 = 0;
		X4 = 0;
		Sig = 0;
		for (i=1; i<= rcount; i++)
			{
			point = itsRealData->GetElement(i);
			JFloat yerr = yAdjError.GetElement(i);
			Y += point.y/(yerr*yerr);
			X += point.x/(yerr*yerr);
			X2 += point.x*point.x/(yerr*yerr);
			YX += point.y*point.x/(yerr*yerr);
			X3 += point.x*point.x*point.x/(yerr*yerr);
			YX2 += point.x*point.x*point.y/(yerr*yerr);
			X4 += point.x*point.x*point.x*point.x/(yerr*yerr);
			Sig += 1/(yerr*yerr);
			itsRealCount++;
			}
		JFloat cv1 = 0, cv2 = 0, cv3 = 0;
		for (i=1; i<= rcount; i++)
			{
			point = itsRealData->GetElement(i);
			JFloat syi = yAdjError.GetElement(i);
			JFloat yi = point.y;
			JFloat xi = point.x;
			for (j = 1; j <= rcount; j++)
				{
				point = itsRealData->GetElement(j);
				JFloat syj = yAdjError.GetElement(j);
				JFloat yj = point.y;
				JFloat xj = point.x;
				cv1 += xi*xj*xj*(xi*yj-yi*xj)/(syi*syi*syj*syj);
				cv2 += (xi*xj*xj*(yi - yj))/(syi*syi*syj*syj);
				cv3 += (xi*xj*xj*(xj - xi))/(syi*syi*syj*syj);
				}
			}
		det = Sig*(X2*X4-X3*X3) + X*(X3*X2-X*X4) + X2*(X*X3-X2*X2);
		tempa = (Y*(X2*X4-X3*X3) + X*(X3*YX2-YX*X4) + X2*(YX*X3-X2*YX2))/det;
		tempb = (Sig*(YX*X4-YX2*X3) + Y*(X3*X2-X*X4) + X2*(X*YX2-YX*X2))/det;
//		tempc = (Sig*(X2*YX2-YX*X3) + X*(YX*X2-X*YX2) + Y*(X*X3-X2*X2))/det;
		tempc = (Sig*cv1 + X*cv2 + Y*cv3)/det;

		JSize index = 1;
		for (i=1; i<=count; i++)
			{
			if (GetDataElement(i, &point))
				{
				JFloat newVal =
					sqrt(point.yerr*point.yerr + (tempb+2*tempc*point.x)*(tempb+2*tempc*point.x)*point.xerr*point.xerr);
				if (newVal == 0)
					{
					newVal = 1;
					}
				yAdjError.SetElement(index, newVal);
				index ++;
				}
			}
		}
	itsRealCount /= 4;
	itsAParameter = tempa;
	itsBParameter = tempb;
	itsCParameter = tempc;
//std::cout <<itsAParameter << " " << parms.GetElement(1,1) << std::endl;
//std::cout <<itsBParameter << " " << parms.GetElement(2,1) << std::endl;
//std::cout <<itsCParameter << " " << parms.GetElement(3,1) << std::endl;
//	itsBParameter = parms.GetElement(2,1);
//	itsCParameter = parms.GetElement(3,1);
	itsChi2 = 0;
	for (i=1; i<= rcount; i++)
		{
		point = itsRealData->GetElement(i);
		JFloat yerr = yAdjError.GetElement(i);
		itsChi2 += pow(point.y - tempa - tempb*point.x - tempc*point.x*point.x,2)/(yerr*yerr);
		}

	itsAErrParameter = 0;
	itsBErrParameter = 0;
	itsCErrParameter = 0;
}

/*********************************************************************************
 QuadMinFit


 ********************************************************************************/

void
GLPlotQuadFit::QuadMinFit()
{
	itsAParameterT = itsAParameter;
	itsBParameterT = itsBParameter;
	itsCParameterT = itsCParameter;

	JVector p(3);
	JMatrix xi(3,3);
	p.SetElement(1, itsAParameterT);
	p.SetElement(2, itsBParameterT);
	p.SetElement(3, itsCParameterT);
	xi.SetElement(1,1,1.0);
	xi.SetElement(2,2,1.0);
	xi.SetElement(3,3,1.0);

	JSize iter;

	MinimizeN(p, xi, &iter, kMinimizeAll);

//	MinimizeChiSqr(itsChi2, kMinimizeAll);

	itsChi2 = ChiSqr(0, kDefaultType);

	itsCParameter = p.GetElement(3);
	itsAParameter = p.GetElement(1);
	itsBParameter = p.GetElement(2);
//	itsCParameter = itsCParameterT;
//	itsAParameter = itsAParameterT;
//	itsBParameter = itsBParameterT;

	itsAErrParameter = CalcError(kAError);
	itsBErrParameter = CalcError(kBError);
	itsCErrParameter = CalcError(kCError);
}

/*********************************************************************************
 MinimizeChiSqr

 enum
{
	kAFixed = 1,
	kBFixed,
	kCFixed,
	kMinimizeAll
};

 ********************************************************************************/

JFloat
GLPlotQuadFit::MinimizeChiSqr
	(
	const JFloat chi2,
	const JIndex type
	)
{
	JFloat chitemp = chi2;
	JSize i = 1;

	JFloat oldA = itsAParameterT;
	JFloat oldB = itsBParameterT;
	JFloat oldC = itsCParameterT;

	while ((i < ITMAX) &&
		((i == 1) ||
		(fabs((oldA-itsAParameterT)/oldA) > 0.000001) ||
		(fabs((oldC-itsCParameterT)/oldC) > 0.000001) ||
		(fabs((oldB-itsBParameterT)/oldB) > 0.000001)))
		{
		i++;
		oldC = itsCParameterT;
		oldB = itsBParameterT;
		oldA = itsAParameterT;

		if (type != kCFixed)
			{
			chitemp = BracketAndMinimize(&itsCParameterT, chitemp, kChiCType);
			}
		if (type != kBFixed)
			{
			chitemp = BracketAndMinimize(&itsBParameterT, chitemp, kChiBType);
			}
		if (type != kAFixed)
			{
			chitemp = BracketAndMinimize(&itsAParameterT, chitemp, kChiAType);
			}
		}
	return chitemp;
}

/*********************************************************************************
 CalcError


 ********************************************************************************/

JFloat
GLPlotQuadFit::CalcError
	(
	const JIndex type
	)
{
	itsAParameterT = itsAParameter;
	itsBParameterT = itsBParameter;
	itsCParameterT = itsCParameter;

	JFloat parameter = 0;
	JFloat *sigParameter = nullptr;
	JIndex fitType = 0;
	JVector p(2);
	JMatrix xi(2,2);
	xi.SetElement(1,1,1.0);
	xi.SetElement(2,2,1.0);

	if (type == kAError)
		{
		sigParameter = &itsAParameterT;
		parameter = itsAParameter;
		fitType = kAFixed;
		p.SetElement(2, itsBParameter);
		p.SetElement(1, itsCParameter);
		}
	else if (type == kBError)
		{
		sigParameter = &itsBParameterT;
		parameter = itsBParameter;
		fitType = kBFixed;
		p.SetElement(1, itsAParameter);
		p.SetElement(2, itsCParameter);
		}
	else if (type == kCError)
		{
		sigParameter = &itsCParameterT;
		parameter = itsCParameter;
		fitType = kCFixed;
		p.SetElement(1, itsAParameter);
		p.SetElement(2, itsBParameter);
		}
	JFloat sig = *sigParameter*(0.000000001);

	*sigParameter = parameter + sig;

	JFloat chiplus 	= sqrt(itsChi2 + 1);
	JIndex i = 0;
	bool ok = true;
	JSize iter;
	JFloat chitemp = MinimizeN(p, xi, &iter, fitType);
	JFloat lastchi;

	do
		{
		if (chitemp > chiplus)
			{
			ok = false;
			}
		else
			{
			lastchi = chitemp;
			*sigParameter = parameter + sig * 10;
			sig *= 10;
			chitemp = sqrt(ChiSqr(0,kDefaultType));//MinimizeN(p, xi, &iter, fitType);
			//std::cout << "1: " << sig << " " << chitemp << std::endl;
			i++;
			}
		}
	while ((i < 20) && ok);

	sig /= 10;
	chitemp = lastchi;

	ok = true;
	i = 2;
	JFloat chi1, chi2, chi3 = lastchi;
	do
		{
		chi1 = chi3;
		*sigParameter = parameter + sig* i;
		chitemp = sqrt(ChiSqr(0,kDefaultType));//MinimizeN(p, xi, &iter, fitType);
		//std::cout << "2: " << sig << " " << chitemp << std::endl;
		chi3 = chitemp;
		if (chitemp > chiplus)
			{
			JFloat x1 = sig*(i-1);
			JFloat x2 = sig*(i-.5);
			JFloat x3 = sig*(i);
			*sigParameter = parameter + x2;
			chi2 = sqrt(ChiSqr(0,kDefaultType));//MinimizeN(p, xi, &iter, fitType);
			JFloat e1 = (chi3*x1*(x1-x2)*x2+x3*(chi1*x2*(x2-x3)+chi2*x1*(-x1+x3)))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e2 = (chi3*(-x1*x1+x2*x2)+chi2*(x1*x1-x3*x3)+chi1*(-x2*x2+x3*x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e3 = (chi3*(x1-x2)+chi1*(x2-x3)+chi2*(-x1+x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat tsig = fabs((-e2+JSign(parameter)*sqrt(e2*e2+4*e3*(chiplus-e1)))/2/e3);
			sig = tsig;
			//std::cout << "3: " << sig << " " << chitemp << std::endl;
			ok = false;
			}
		i++;
		}
	while ((i <= 10) && ok);
	sig *= 10;
	if (type == kAError)
		{
		p.SetElement(2, itsBParameter);
		p.SetElement(1, itsCParameter);
		}
	else if (type == kBError)
		{
		p.SetElement(1, itsAParameter);
		p.SetElement(2, itsCParameter);
		}
	else if (type == kCError)
		{
		p.SetElement(1, itsAParameter);
		p.SetElement(2, itsBParameter);
		}
	xi.SetElement(1,1,1.0);
	xi.SetElement(2,2,1.0);
	xi.SetElement(1,2,0.0);
	xi.SetElement(2,1,0.0);

//std::cout << "Starting sig: " << sig << std::endl;
	*sigParameter = parameter + sig;

	chitemp = MinimizeN(p, xi, &iter, fitType);
//std::cout << "Iter: " << iter << std::endl;
//std::cout << "1a " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;
//	chitemp = MinimizeN(p, xi, &iter, fitType);
//std::cout << "1b " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;

	i = 0;
	ok = true;
	do
		{
		if (chitemp > chiplus)
			{
			ok = false;
			}
		else
			{
			lastchi = chitemp;
			*sigParameter = parameter + sig * 10;
			sig *= 10;
			chitemp = MinimizeN(p, xi, &iter, fitType);//MinimizeChiSqr(chitemp, fitType);
		//std::cout << "Iter: " << iter << std::endl;
		//std::cout << "2a " << sig << " " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;
//			chitemp = MinimizeN(p, xi, &iter, fitType);
//		std::cout << "2b " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;
			i++;
			}
		}
	while ((i < 20) && ok);

	sig /= 10;
	chitemp = lastchi;

	ok = true;
	i = 2;
	chi3 = lastchi;
	do
		{
		chi1 = chi3;
		*sigParameter = parameter + sig* i;
		chitemp = MinimizeN(p, xi, &iter, fitType);//MinimizeChiSqr(chitemp, fitType);
	//std::cout << "Iter: " << iter << std::endl;
	//std::cout << "3a " << sig << " " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;
//		chitemp = MinimizeN(p, xi, &iter, fitType);
//	std::cout << "3b " << sig << " " << p.GetElement(1) <<" " << p.GetElement(2) << " " << chitemp << std::endl;
		chi3 = chitemp;
		if (chitemp > chiplus)
			{
			JFloat x1 = sig*(i-1);
			JFloat x2 = sig*(i-.5);
			JFloat x3 = sig*(i);
		//std::cout << x1 << " " << x2 << " " << x3 << std::endl;
			*sigParameter = parameter + x2;
			chi2 = MinimizeN(p, xi, &iter, fitType);//MinimizeChiSqr(chitemp, fitType);
		//std::cout << "Iter: " << iter << std::endl;
//			chi2 = MinimizeN(p, xi, &iter, fitType);
		//std::cout << chi1 << " " << chi2 << " " << chi3 << std::endl;
			JFloat e1 = (chi3*x1*(x1-x2)*x2+x3*(chi1*x2*(x2-x3)+chi2*x1*(-x1+x3)))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e2 = (chi3*(-x1*x1+x2*x2)+chi2*(x1*x1-x3*x3)+chi1*(-x2*x2+x3*x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e3 = (chi3*(x1-x2)+chi1*(x2-x3)+chi2*(-x1+x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat tsig = fabs((-e2+JSign(parameter)*sqrt(e2*e2+4*e3*(chiplus-e1)))/2/e3);
			if (tsig > x3)
				{
				tsig = x2;
//				std::cout << x1 << " " << x2 << " " << x3 << std::endl;
//				std::cout << chi1 << " " << chi2 << " " << chi3 << std::endl;
				}
			//std::cout << "*****" << tsig << "*****" << std::endl;
			return tsig;
			ok = false;
			}
		i++;
		}
	while ((i <= 10) && ok);
	return 0;

}



/*********************************************************************************
 ChiSqr

 ********************************************************************************/

JFloat
GLPlotQuadFit::ChiSqr
	(
	JFloat 			Bt,
	const JIndex 	type
	)
{
	J2DDataPoint point;
	JSize rcount = itsRealData->GetElementCount();
	JFloat c = 0;

	for (JSize i = 1; i <= rcount; i++)
		{
		point = itsRealData->GetElement(i);
		JFloat sy = point.yerr;
		JFloat sx = point.xerr;
		if ((sy == 0) && (sx == 0))
			{
			sy = 1;
			}
		if (type == kChiCType)
			{
			JFloat e = pow(point.xerr * (2*Bt*point.x + itsBParameterT),2) + sy * sy;
			c += pow(point.y - itsAParameterT - itsBParameterT*point.x - Bt*point.x*point.x,2)/e;
			}
		else if (type == kChiBType)
			{
			JFloat e = pow(point.xerr * (2*itsCParameterT*point.x + Bt),2) + sy * sy;
			c += pow(point.y - itsAParameterT - Bt*point.x - itsCParameterT*point.x*point.x, 2)/e;
			}
		else if (type == kChiAType)
			{
			JFloat e = pow(point.xerr * (2*itsCParameterT*point.x + itsBParameterT),2) + sy * sy;
			c += pow(point.y - Bt - itsBParameterT*point.x - itsCParameterT*point.x*point.x, 2)/e;
			}
		else if (type == kDefaultType)
			{
			JFloat e = pow(point.xerr * (2*itsCParameterT*point.x + itsBParameterT),2) + sy * sy;
			c += pow(point.y - itsAParameterT - itsBParameterT*point.x - itsCParameterT*point.x*point.x, 2)/e;
			}
		}
		return c;

}

/*********************************************************************************
 Function

 ********************************************************************************/

JFloat
GLPlotQuadFit::Function
	(
	JFloat	 		Bt,
	const JIndex 	type
	)
{
	assert(itsP != nullptr);
	JVector xt(*itsP);
	xt = *itsP + Bt * *itsXi;
	JFloat value = FunctionN(xt, itsCurrentType);
	return value;
}

/*********************************************************************************
 FunctionN

 ********************************************************************************/

JFloat
GLPlotQuadFit::FunctionN
	(
	JVector& 		parameters,
	const JIndex 	type
	)
{
	J2DDataPoint point;
	JSize rcount = itsRealData->GetElementCount(); 
	JFloat c = 0;

	JFloat A;
	JFloat B;
	JFloat C;

	if (type == kAFixed)
		{
		A = itsAParameterT;
		B = parameters.GetElement(2);
		C = parameters.GetElement(1);
		}
	else if (type == kBFixed)
		{
		B = itsBParameterT;
		A = parameters.GetElement(1);
		C = parameters.GetElement(2);
		}
	else if (type == kCFixed)
		{
		C = itsCParameterT;
		A = parameters.GetElement(1);
		B = parameters.GetElement(2);
		}
	else
		{
		A = parameters.GetElement(1);
		B = parameters.GetElement(2);
		C = parameters.GetElement(3);
		}

	for (JSize i = 1; i <= rcount; i++)
		{
		point = itsRealData->GetElement(i);
		JFloat sy = point.yerr;
		JFloat sx = point.xerr;
		if ((sy == 0) && (sx == 0))
			{
			sy = 1;
			}
		JFloat e = pow(point.xerr * (2*C*point.x + B),2) + sy * sy;
		c += pow(point.y - A - B*point.x - C*point.x*point.x,2)/e;
		}
		return sqrt(c);

}

/*********************************************************************************
 BracketAndMinimize


 ********************************************************************************/

JFloat
GLPlotQuadFit::BracketAndMinimize
	(
	JFloat* 		parameter,
	const JFloat 	chitemp,
	const JIndex 	type
	)
{
	JFloat factor=0.01, small=TOLL, bmin, bmax;
	JFloat cbmax, cbmin, btemp;
	int iter;

	btemp = *parameter;

	bmax = btemp*(1+factor);
	bmin = btemp*(1-factor);

	cbmax = Function(bmax, type);
	cbmin = Function(bmin, type);

	iter = 1;
	while (iter < 100)
		{
		cbmax = Function(bmax, type);
		cbmin = Function(bmin, type);
		if ((cbmin > chitemp) && (cbmax > chitemp))
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

	Minimize(bmin, btemp, bmax, parameter, type);
	return Function(*parameter, type);
}

/*********************************************************************************
 Minimize


 ********************************************************************************/

JFloat
GLPlotQuadFit::Minimize
	(
	JFloat 			ax,
	JFloat 			bx,
	JFloat 			cx,
	JFloat* 		xmin,
	const JIndex	type
	)
{
	JFloat oldstep, x, w, v, fx, fw, fv, middle, tol2, ymin, r, q;
	JFloat p, steptemp, tol1, step, low, high, u, fu;
	JSize iter;

	oldstep = 0.0;
	x = bx;
	w = bx;
	v = bx;


	fx = Function(bx, type);
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
			return fx;
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
		fu = Function(u, type);
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
	return fx;
}

/*********************************************************************************
 MinimizeN


 ********************************************************************************/

JFloat
GLPlotQuadFit::MinimizeN
	(
	JVector& 		p,
	JMatrix& 		xi,
	JSize 			*iter,
	const JIndex	type
	)
{
	JSize i,ibig;
	JFloat del,fp,fptt,t, fret;
	JIndex n = p.GetDimensionCount();

	JVector pt(n);
	JVector ptt(n);
	JVector xit(n);

	fret = FunctionN(p, type);
	pt = p;

	for (*iter = 1; *iter <= ITMAX; ++(*iter))
		{
		fp = fret;
		ibig=0;
		del=0.0;
		for (i = 1; i <= n; i++)
			{
			xit = xi.GetColVector(i);
			fptt = fret;
			fret = LinearMinimization(p, xit, type);
			if (fabs(fptt-fret) > del)
				{
				del=fabs(fptt-fret);
				ibig=i;
				}
			}
		if (2.0*fabs(fp-fret) <= TOLL*(fabs(fp)+fabs(fret)))
			{
			return fret;
			}
		ptt = 2.0*p - pt;
		xit = p - pt;
		pt = p;
		fptt = FunctionN(ptt, type);
		if (fptt < fp)
			{
			t = 2.0*(fp - 2.0*fret+fptt) * (fp-fret-del)*(fp-fret-del) -del*(fp-fptt)*(fp-fptt);
			if (t < 0.0)
				{
				fret = LinearMinimization(p, xit, type);
				xi.SetColVector(ibig, xi.GetColVector(n));
				xi.SetColVector(n, xit);
				}
			}
		}
	return fret;
}

/*********************************************************************************
 LinearMinimization


 ********************************************************************************/

JFloat
GLPlotQuadFit::LinearMinimization
	(
	JVector& 		p,
	JVector& 		xi,
	const JIndex	type
	)
{
	JFloat xx,xmin,fx,fb,fa,bx,ax, fret;

	assert (itsP == nullptr);
	itsP = jnew JVector(p);
	assert(itsP != nullptr);

	assert (itsXi == nullptr);
	itsXi = jnew JVector(xi);
	assert(itsXi != nullptr);

	itsCurrentType = type;

	ax=0.0;
	xx=1.0;
	Bracket(&ax,&xx,&bx,&fa,&fx,&fb,k1DimType);
	fret = Minimize(ax,xx,bx,&xmin, k1DimType);
	xi *= xmin;
	p += xi;

	jdelete itsP;
	itsP = nullptr;
	jdelete itsXi;
	itsXi = nullptr;

	return fret;
}

/*********************************************************************************
 Bracket


 ********************************************************************************/

void
GLPlotQuadFit::Bracket
	(
	JFloat 			*ax,
	JFloat 			*bx,
	JFloat 			*cx,
	JFloat 			*fa,
	JFloat 			*fb,
	JFloat 			*fc,
	const JIndex	type
	)
{
	JFloat ulim,u,r,q,fu,dum;

	*fa = Function(*ax, type);
	*fb = Function(*bx, type);

	if (*fb > *fa)
		{
		Shift(dum,*ax,*bx,dum);
		Shift(dum,*fb,*fa,dum);
		}
	*cx = (*bx)+GOLD*(*bx-*ax);
	*fc = Function(*cx, type);

	while (*fb > *fc)
		{
		r = (*bx-*ax)*(*fb-*fc);
		q = (*bx-*cx)*(*fb-*fa);
		int sign;
		if (q-r > 0.0)
			{
			sign = 1;
			}
		else
			{
			sign = -1;
			}
		u = (*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
			(2.0*sign*JMax(fabs(q-r),TINY));
		ulim = (*bx)+GLIMIT*(*cx-*bx);
		if ((*bx-u)*(u-*cx) > 0.0)
			{
			fu = Function(u, type);
			if (fu < *fc)
				{
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
				return;
				}
			else if (fu > *fb)
				{
				*cx = u;
				*fc = fu;
				return;
				}
			u = (*cx)+GOLD*(*cx-*bx);
			fu = Function(u, type);
			}
		else if ((*cx-u)*(u-ulim) > 0.0)
			{
			fu = Function(u, type);
			if (fu < *fc)
				{
				JFloat temp = *cx+GOLD*(*cx-*bx);
				Shift(*bx,*cx,u,temp);
				temp = Function(u, type);
				Shift(*fb,*fc,fu,temp);
				}
			}
		else if ((u-ulim)*(ulim-*cx) >= 0.0)
			{
			u = ulim;
			fu = Function(u, type);
			}
		else
			{
			u = (*cx)+GOLD*(*cx-*bx);
			fu = Function(u, type);
			}
		Shift(*ax,*bx,*cx,u);
		Shift(*fa,*fb,*fc,fu);
		}
}

void
GLPlotQuadFit::Shift
	(
	JFloat& a,
	JFloat& b,
	JFloat& c,
	JFloat& d
	)
{
	a = b;
	b = c;
	c = d;
}

/*********************************************************************************
 SetFunctionName


 ********************************************************************************/

void
GLPlotQuadFit::SetFunctionName
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
GLPlotQuadFit::DataElementValid
	(
	const JIndex index
	)
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	data->GetElement(index, &point);

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
GLPlotQuadFit::GetDataElement
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
	return true;
}

/*****************************************************************************
 AdjustDiffData


 *****************************************************************************/

void
GLPlotQuadFit::AdjustDiffData()
{
}
