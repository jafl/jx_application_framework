/*********************************************************************************
 GloveCurveStats.h
 
	Interface for the GloveCurveStats struct.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_GLCurveStats
#define _H_GLCurveStats

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

struct GloveCurveStats
{
	GCurveType		type;
	JIndex			provider;
	JIndex			fitNumber;
	GCurveFitType	fitType;
};

#endif
