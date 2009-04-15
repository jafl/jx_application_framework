/******************************************************************************
 JXPasswordInput.h

	Interface for the JXPasswordInput class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPasswordInput
#define _H_JXPasswordInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JXPasswordInput : public JXInputField
{
public:

	JXPasswordInput(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXPasswordInput();

	const JString&	GetPassword() const;
	void			ClearPassword();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	TERefreshRect(const JRect& rect);
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

private:

	JString*	itsFakeString;

private:

	// not allowed

	JXPasswordInput(const JXPasswordInput& source);
	const JXPasswordInput& operator=(const JXPasswordInput& source);
};

/******************************************************************************
 GetPassword

 ******************************************************************************/

inline const JString&
JXPasswordInput::GetPassword()
	const
{
	return GetText();
}

#endif
