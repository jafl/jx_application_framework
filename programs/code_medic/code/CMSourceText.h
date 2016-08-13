/******************************************************************************
 CMSourceText.h

	Interface for the CMSourceText class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMSourceText
#define _H_CMSourceText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMTextDisplayBase.h"
#include "CBTextFileType.h"
#include <JTEStyler.h>

class CMSourceDirector;
class CMCommandDirector;
class CBStylerBase;

class CMSourceText : public CMTextDisplayBase
{
public:

	CMSourceText(CMSourceDirector* srcDir, CMCommandDirector* cmdDir,
					JXMenuBar* menuBar,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CMSourceText();

	void	SetFileType(const CBTextFileType type);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CMSourceDirector*				itsSrcDir;
	CMCommandDirector*				itsCmdDir;
	CBStylerBase*					itsStyler;
	JArray<JTEStyler::TokenData>*	itsTokenStartList;		// NULL if styling is turned off
	JIndex							itsFirstSearchMenuItem;	// index of first item added to Search menu
	JSize							itsLastClickCount;

private:

	void		UpdateCustomSearchMenuItems();
	JBoolean	HandleCustomSearchMenuItems(const JIndex index);

	// not allowed

	CMSourceText(const CMSourceText& source);
	const CMSourceText& operator=(const CMSourceText& source);

};

#endif
