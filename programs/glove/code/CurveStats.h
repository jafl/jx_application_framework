/*********************************************************************************
 CurveStats.h
 
	Interface for the CurveStats struct.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_CurveStats
#define _H_CurveStats

#include <jTypes.h>

enum GCurveType
{
	kGDataCurve = 1,
	kGFunctionCurve,
	kGFitCurve,
	kGDiffCurve
};

enum GCurveFitType
{
	kGLinearFit = 1,
	kGQuadFit,
	kGExpFit,
	kGPowFit,
	kGModFit,
	kGProxyFit
};

struct CurveStats
{
	GCurveType		type;
	JIndex			provider;
	JIndex			fitNumber;
	GCurveFitType	fitType;
};

#endif
