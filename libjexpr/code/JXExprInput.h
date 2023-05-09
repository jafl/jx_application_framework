/******************************************************************************
 JXExprInput.h

	Interface for the JXExprInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprInput
#define _H_JXExprInput

#include <jx-af/jx/JXInputField.h>

class JXTextMenu;

class JXExprInput : public JXInputField
{
public:

	JXExprInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~JXExprInput() override;

	const JString&	GetVarName() const;
	void			SetVarName(const JString& str);

	static JXTextMenu*	CreateFontMenu(JXContainer* enclosure);
	void				SetFontMenu(JXTextMenu* menu);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

private:

	JXTextMenu*	itsFontMenu;	// can be nullptr
	bool		itsGreekFlag;

private:

	void	UpdateFontMenu();
	void	HandleFontMenu(const JIndex item);
};


/******************************************************************************
 GetVarName

 ******************************************************************************/

inline const JString&
JXExprInput::GetVarName()
	const
{
	return GetText().GetText();
}

/******************************************************************************
 SetVarName

 ******************************************************************************/

inline void
JXExprInput::SetVarName
	(
	const JString& str
	)
{
	GetText()->SetText(str);
}

#endif
