/******************************************************************************
 CMFileDragSource.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMFileDragSource
#define _H_CMFileDragSource

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

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

private:

	CMSourceDirector*	itsDoc;		// owns us

private:

	// not allowed

	CMFileDragSource(const CMFileDragSource& source);
	const CMFileDragSource& operator=(const CMFileDragSource& source);
};

#endif
