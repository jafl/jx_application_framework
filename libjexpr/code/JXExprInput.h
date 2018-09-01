/******************************************************************************
 JXExprInput.h

	Interface for the JXExprInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprInput
#define _H_JXExprInput

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
	void	SetVarName(const JString& str);

	static JXTextMenu*	CreateFontMenu(JXContainer* enclosure);
	void				SetFontMenu(JXTextMenu* menu);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*	itsFontMenu;	// can be nullptr

private:

	void	UpdateFontMenu();
	void	HandleFontMenu(const JIndex item);

	// not allowed

	JXExprInput(const JXExprInput& source);
	const JXExprInput& operator=(const JXExprInput& source);
};

#endif
