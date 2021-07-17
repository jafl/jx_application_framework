/******************************************************************************
 CBKeyScriptInput.h

	Interface for the CBKeyScriptInput class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBKeyScriptInput
#define _H_CBKeyScriptInput

#include <JXInputField.h>

class CBKeyScriptInput : public JXInputField
{
public:

	CBKeyScriptInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CBKeyScriptInput();

protected:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXInputField::StyledText(false, fontManager)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;
	};

private:

	// not allowed

	CBKeyScriptInput(const CBKeyScriptInput& source);
	const CBKeyScriptInput& operator=(const CBKeyScriptInput& source);
};

#endif
