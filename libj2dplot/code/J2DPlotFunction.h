/*********************************************************************************
 J2DPlotFunction.h

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DPlotJFunction
#define _H_J2DPlotJFunction

#include "jx-af/j2dplot/J2DPlotFunctionBase.h"

class JVariableList;
class JFunction;
class JFontManager;

class J2DPlotFunction : public J2DPlotFunctionBase
{
public:

	static bool Create(	J2DPlotFunction **plotfunction, J2DPlotWidget* plot,
						JVariableList* varList, const JString& function,
						const JIndex xIndex, const JFloat xMin, const JFloat xMax);

	J2DPlotFunction(J2DPlotWidget* plot, JVariableList* varList, JFunction* f,
					const bool ownsFn, const JIndex xIndex,
					const JFloat xMin, const JFloat xMax);

	virtual ~J2DPlotFunction();

	const JVariableList*	GetVariableList() const;
	const JFunction&		GetFunction() const;
	JString			GetFunctionString() const override;
	bool			GetYValue(const JFloat x, JFloat* y) const override;

	void	SetFunction(JVariableList* varList, JFunction* f,
						const bool ownsFn, const JIndex xIndex,
						const JFloat xMin, const JFloat xMax);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JVariableList*	itsVarList;		// not owned
	JFunction*		itsFunction;
	bool			itsOwnsFnFlag;	// true => delete itsFunction when destructed
	JIndex			itsXIndex;
};


/*********************************************************************************
 GetVariableList

 ********************************************************************************/

inline const JVariableList*
J2DPlotFunction::GetVariableList()
	const
{
	return itsVarList;
}

/*********************************************************************************
 GetFunction

 ********************************************************************************/

inline const JFunction&
J2DPlotFunction::GetFunction()
	const
{
	return *itsFunction;
}

#endif
