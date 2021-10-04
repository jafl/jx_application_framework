/******************************************************************************
 JXSaveFileInput.h

	Interface for the JXSaveFileInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSaveFileInput
#define _H_JXSaveFileInput

#include "jx-af/jx/JXInputField.h"

class JXSaveFileInput : public JXInputField
{
public:

	JXSaveFileInput(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXSaveFileInput();

	static bool	WillAllowSpace();
	static void		ShouldAllowSpace(const bool allow);

protected:

	void	HandleFocusEvent() override;

private:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXInputField::StyledText(false, fontManager)
		{ };

		protected:

		bool	NeedsToFilterText(const JString& text) const override;
		bool	FilterText(JString* text, JRunArray<JFont>* style) override;
	};

private:

	static bool	theAllowSpaceFlag;

private:

	static bool	IsCharacterInWord(const JUtf8Character& c);
};

/******************************************************************************
 Allow space (static)

 ******************************************************************************/

inline bool
JXSaveFileInput::WillAllowSpace()
{
	return theAllowSpaceFlag;
}

inline void
JXSaveFileInput::ShouldAllowSpace
	(
	const bool allow
	)
{
	theAllowSpaceFlag = allow;
}

#endif
