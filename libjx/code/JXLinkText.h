/******************************************************************************
 JXLinkText.h

	Interface for the JXLinkText class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXLinkText
#define _H_JXLinkText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>

class JXLinkText : public JXTEBase
{
public:

	JXLinkText(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXLinkText();

protected:

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JSize		GetLinkCount() const = 0;
	virtual JIndexRange	GetLinkRange(const JIndex index) const = 0;
	virtual void		LinkClicked(const JIndex index) = 0;

private:

	JCursorIndex	itsLinkCursor;
	JIndex			itsMouseDownIndex;		// used while dragging

private:

	// not allowed

	JXLinkText(const JXLinkText& source);
	const JXLinkText& operator=(const JXLinkText& source);
};

#endif
