/******************************************************************************
 THX3DPlotDirector.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THX3DPlotDirector
#define _H_THX3DPlotDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JMatrix;
class JFunction;
class J3DUniverse;
class JXTextMenu;
class THX3DPlotWidget;
class THX3DPlotFunctionDialog;
class THXVarList;

class THX3DPlotDirector : public JXWindowDirector
{
public:

	THX3DPlotDirector(JXDirector* supervisor);
	THX3DPlotDirector(istream& input, const JFileVersion vers,
					  JXDirector* supervisor, THXVarList* varList);

	virtual ~THX3DPlotDirector();

	void	SetFunction(THXVarList* varList, const JFunction& f,
						const JCharacter* name,
						const JFloat xMin, const JFloat xMax, const JSize xCount,
						const JFloat yMin, const JFloat yMax, const JSize yCount);

	void	WriteState(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	J3DUniverse*	itsUniverse;
	JXTextMenu*		itsActionsMenu;
	JXTextMenu*		itsOptionsMenu;
	JXTextMenu*		itsHelpMenu;

	JFloat	itsXMin, itsXMax, itsYMin, itsYMax;
	JSize	itsXCount, itsYCount;

	THXVarList*	itsVarList;		// not owned
	JFunction*	itsFunction;

	THX3DPlotFunctionDialog*	itsEditFnDialog;	// NULL unless editing

// begin JXLayout

    THX3DPlotWidget* its3DPlot;

// end JXLayout

private:

	void	THX3DPlotDirectorX(THXVarList* varList);
	void	BuildWindow();

	void	ComputeFunction();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	UpdateOptionsMenu();
	void	HandleOptionsMenu(const JIndex index);

	void	EditFunction();
	void	UpdateFunction();

	// not allowed

	THX3DPlotDirector(const THX3DPlotDirector& source);
	const THX3DPlotDirector& operator=(const THX3DPlotDirector& source);
};

#endif
