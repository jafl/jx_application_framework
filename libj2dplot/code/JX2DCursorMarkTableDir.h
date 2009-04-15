/******************************************************************************
 JX2DCursorMarkTableDir.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DCursorMarkTableDir
#define _H_JX2DCursorMarkTableDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class J2DPlotWidget;
class JXTextButton;
class JX2DCursorMarkTable;

class JX2DCursorMarkTableDir : public JXWindowDirector
{
public:

	JX2DCursorMarkTableDir(JXWindowDirector* supervisor, J2DPlotWidget* plot);

	virtual ~JX2DCursorMarkTableDir();

	JX2DCursorMarkTable*	GetTable();

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JX2DCursorMarkTable*	itsTable;

// begin JXLayout

    JXTextButton* itsCloseButton;

// end JXLayout

private:

	void	BuildWindow(J2DPlotWidget* plot);

	// not allowed

	JX2DCursorMarkTableDir(const JX2DCursorMarkTableDir& source);
	const JX2DCursorMarkTableDir& operator=(const JX2DCursorMarkTableDir& source);
};

/******************************************************************************
 GetTable

 ******************************************************************************/

inline JX2DCursorMarkTable*
JX2DCursorMarkTableDir::GetTable()
{
	return itsTable;
}

#endif
