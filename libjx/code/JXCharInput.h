/******************************************************************************
 JXCharInput.h

	Interface for the JXCharInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCharInput
#define _H_JXCharInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JXCharInput : public JXInputField
{
public:

	JXCharInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXCharInput();

	JCharacter	GetCharacter() const;
	void		SetCharacter(const JCharacter c);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);

private:

	// not allowed

	JXCharInput(const JXCharInput& source);
	const JXCharInput& operator=(const JXCharInput& source);
};

/******************************************************************************
 SetCharacter

 ******************************************************************************/

inline void
JXCharInput::SetCharacter
	(
	const JCharacter c
	)
{
	const JCharacter s[2] = { c, '\0' };
	SetText(s);
}

#endif
