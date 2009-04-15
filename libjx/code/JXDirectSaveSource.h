/******************************************************************************
 JXDirectSaveSource.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDirectSaveSource
#define _H_JXDirectSaveSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXImageWidget.h>

class JXSaveFileDialog;
class JXInputField;

class JXDirectSaveSource : public JXImageWidget
{
public:

	JXDirectSaveSource(JXSaveFileDialog* dialog, JXInputField* nameInput,
					   JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXDirectSaveSource();

	static void	Init(JXWindow* window, const JCharacter* fileName);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

private:

	JXSaveFileDialog*	itsDialog;		// owns us
	JXInputField*		itsNameInput;	// not owned

private:

	// not allowed

	JXDirectSaveSource(const JXDirectSaveSource& source);
	const JXDirectSaveSource& operator=(const JXDirectSaveSource& source);
};

#endif
