/******************************************************************************
 JXSaveFileInput.h

	Interface for the JXSaveFileInput class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSaveFileInput
#define _H_JXSaveFileInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JXSaveFileInput : public JXInputField
{
public:

	JXSaveFileInput(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXSaveFileInput();

	static JBoolean	WillAllowSpace();
	static void		ShouldAllowSpace(const JBoolean allow);

protected:

	virtual void	HandleFocusEvent();

	virtual JBoolean	NeedsToFilterText(const JCharacter* text) const;
	virtual JBoolean	FilterText(JString* text, JRunArray<Font>* style);

private:

	static JBoolean	itsAllowSpaceFlag;

private:

	static JBoolean	IsCharacterInWord(const JString& text, const JIndex charIndex);

	// not allowed

	JXSaveFileInput(const JXSaveFileInput& source);
	const JXSaveFileInput& operator=(const JXSaveFileInput& source);
};

/******************************************************************************
 Allow space (static)

 ******************************************************************************/

inline JBoolean
JXSaveFileInput::WillAllowSpace()
{
	return itsAllowSpaceFlag;
}

inline void
JXSaveFileInput::ShouldAllowSpace
	(
	const JBoolean allow
	)
{
	itsAllowSpaceFlag = allow;
}

#endif
