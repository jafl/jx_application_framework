/******************************************************************************
 CBCommandPathInput.h

	Copyright © 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCommandPathInput
#define _H_CBCommandPathInput

#include <JXPathInput.h>

class CBCommandPathInput : public JXPathInput
{
public:

	CBCommandPathInput(JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBCommandPathInput();

	virtual bool	InputValid();
	virtual bool	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

	static JColorID	GetTextColor(const JString& path, const JString& base,
								 const bool requireWrite);

protected:

	class StyledText : public JXPathInput::StyledText
	{
		public:

		StyledText(CBCommandPathInput* field, JFontManager* fontManager)
			:
			JXPathInput::StyledText(field, fontManager)
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

	CBCommandPathInput(const CBCommandPathInput& source);
	const CBCommandPathInput& operator=(const CBCommandPathInput& source);
};

#endif
