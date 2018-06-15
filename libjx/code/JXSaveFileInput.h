/******************************************************************************
 JXSaveFileInput.h

	Interface for the JXSaveFileInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSaveFileInput
#define _H_JXSaveFileInput

#include "JXInputField.h"

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

	virtual void	HandleFocusEvent() override;

private:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXInputField::StyledText(kJFalse, fontManager)
		{ };

		protected:

		virtual JBoolean	NeedsToFilterText(const JString& text, const JRunArray<JFont>& style) const override;
		virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style) override;
	};

private:

	static JBoolean	theAllowSpaceFlag;

private:

	static JBoolean	IsCharacterInWord(const JUtf8Character& c);

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
	return theAllowSpaceFlag;
}

inline void
JXSaveFileInput::ShouldAllowSpace
	(
	const JBoolean allow
	)
{
	theAllowSpaceFlag = allow;
}

#endif
