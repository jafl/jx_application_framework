/******************************************************************************
 JXExprInput.h

	Interface for the JXExprInput class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXExprInput
#define _H_JXExprInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JXTextMenu;

class JXExprInput : public JXInputField
{
public:

	JXExprInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXExprInput();

	JString	GetVarName() const;
	void	SetVarName(const JCharacter* str);

	static JXTextMenu*	CreateFontMenu(JXContainer* enclosure);
	void				SetFontMenu(JXTextMenu* menu);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*	itsFontMenu;	// can be NULL

private:

	void	UpdateFontMenu();
	void	HandleFontMenu(const JIndex item);

	// not allowed

	JXExprInput(const JXExprInput& source);
	const JXExprInput& operator=(const JXExprInput& source);
};

#endif
