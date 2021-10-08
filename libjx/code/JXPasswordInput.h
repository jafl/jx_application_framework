/******************************************************************************
 JXPasswordInput.h

	Interface for the JXPasswordInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPasswordInput
#define _H_JXPasswordInput

#include "jx-af/jx/JXInputField.h"

class JXPasswordInput : public JXInputField
{
public:

	JXPasswordInput(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~JXPasswordInput() override;

	const JString&	GetPassword() const;
	void			ClearPassword();

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	void	TERefreshRect(const JRect& rect) override;
	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;
};

/******************************************************************************
 GetPassword

 ******************************************************************************/

inline const JString&
JXPasswordInput::GetPassword()
	const
{
	return GetText().GetText();
}

#endif
