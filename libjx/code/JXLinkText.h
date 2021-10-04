/******************************************************************************
 JXLinkText.h

	Interface for the JXLinkText class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXLinkText
#define _H_JXLinkText

#include "jx-af/jx/JXTEBase.h"

class JXLinkText : public JXTEBase
{
public:

	JXLinkText(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXLinkText();

protected:

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JSize			GetLinkCount() const = 0;
	virtual JCharacterRange	GetLinkRange(const JIndex index) const = 0;
	virtual void			LinkClicked(const JIndex index) = 0;

private:

	JCursorIndex	itsLinkCursor;
	JIndex			itsMouseDownIndex;		// used while dragging
};

#endif
