/******************************************************************************
 CMFileDragSource.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMFileDragSource
#define _H_CMFileDragSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXImageWidget.h>

class CMSourceDirector;

class CMFileDragSource : public JXImageWidget
{
public:

	CMFileDragSource(CMSourceDirector* doc, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMFileDragSource();

protected:

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

private:

	CMSourceDirector*	itsDoc;		// owns us

private:

	// not allowed

	CMFileDragSource(const CMFileDragSource& source);
	const CMFileDragSource& operator=(const CMFileDragSource& source);
};

#endif
