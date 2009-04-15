/******************************************************************************
 JPlotFitProxy.h

	Interface for the JPlotFitProxy class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_JPlotFitProxy
#define _H_JPlotFitProxy


#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotFitFunction.h>

#include <JArray.h>
#include <JString.h>

class GVarList;
class JFunction;

class JPlotFitProxy : public JPlotFitFunction
{
public:

public:

	JPlotFitProxy(JPlotFitFunction* fit,
				  J2DPlotWidget* plot, JPlotDataBase* fitData);
	JPlotFitProxy(J2DPlotWidget* plot, JPlotDataBase* fitData, istream& is);
	virtual ~JPlotFitProxy();

	void	WriteData(ostream& os);

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString		GetFitFunctionString() const;

	virtual JString		GetFunctionString() const;
	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const;

protected:

private:

	GVarList*		itsParms;
	JArray<JFloat>*	itsErrors;
	JBoolean		itsHasGOF;
	JString			itsGOFName;
	JFloat			itsGOF;
	JString			itsFnString;
	JFunction*		itsFn;

private:

	// not allowed

	JPlotFitProxy(const JPlotFitProxy& source);
	const JPlotFitProxy& operator=(const JPlotFitProxy& source);

public:
};

#endif
