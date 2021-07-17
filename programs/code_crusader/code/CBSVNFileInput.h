/******************************************************************************
 CBSVNFileInput.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSVNFileInput
#define _H_CBSVNFileInput

#include <JXFileInput.h>

class CBSVNFileInput : public JXFileInput
{
public:

	CBSVNFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CBSVNFileInput();

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(CBSVNFileInput* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager)
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

	CBSVNFileInput(const CBSVNFileInput& source);
	const CBSVNFileInput& operator=(const CBSVNFileInput& source);
};

#endif
