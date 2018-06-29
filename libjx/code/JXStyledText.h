/******************************************************************************
 JXStyledText.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyledText
#define _H_JXStyledText

#include <JStyledText.h>

class JFontManager;

class JXStyledText : public JStyledText
{
public:

	JXStyledText(const JBoolean useMultipleUndo, const JBoolean pasteStyledText,
				 JFontManager* fontManager);
	JXStyledText(const JXStyledText& source);

	virtual ~JXStyledText();

protected:

	virtual JBoolean	NeedsToAdjustFontToDisplayGlyphs(const JString& text, const JRunArray<JFont>& style) const;
	virtual JBoolean	AdjustFontToDisplayGlyphs(const TextRange& range,
												  const JString& text,
												  JRunArray<JFont>* style);

private:

	JFontManager*	itsFontManager;		// can be nullptr
};

#endif
