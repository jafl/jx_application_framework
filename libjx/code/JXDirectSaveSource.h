/******************************************************************************
 JXDirectSaveSource.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDirectSaveSource
#define _H_JXDirectSaveSource

#include "JXImageWidget.h"

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

	~JXDirectSaveSource() override;

	static void	Init(JXWindow* window, const JString& fileName);

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
					const JXKeyModifiers& modifiers) override;
	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;
	void	HandleDNDResponse(const JXContainer* target,
							  const bool dropAccepted, const Atom action) override;

private:

	JXSaveFileDialog*	itsDialog;		// owns us
	JXInputField*		itsNameInput;	// not owned
};

#endif
