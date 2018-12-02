/******************************************************************************
 GLPlotFitProxy.h

	Interface for the GLPlotFitProxy class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLPlotFitProxy
#define _H_GLPlotFitProxy


#include <GLPlotFitFunction.h>

#include <JArray.h>
#include <JString.h>

class GLVarList;
class JFunction;

class GLPlotFitProxy : public GLPlotFitFunction
{
public:

public:

	GLPlotFitProxy(GLPlotFitFunction* fit,
				  J2DPlotWidget* plot, JPlotDataBase* fitData);
	GLPlotFitProxy(J2DPlotWidget* plot, JPlotDataBase* fitData, std::istream& is);
	virtual ~GLPlotFitProxy();

	void	WriteData(std::ostream& os);

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

	GLVarList*		itsParms;
	JArray<JFloat>*	itsErrors;
	JBoolean		itsHasGOF;
	JString			itsGOFName;
	JFloat			itsGOF;
	JString			itsFnString;
	JFunction*		itsFn;

private:

	// not allowed

	GLPlotFitProxy(const GLPlotFitProxy& source);
	const GLPlotFitProxy& operator=(const GLPlotFitProxy& source);

public:
};

#endif
