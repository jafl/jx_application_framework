/******************************************************************************
 JXStyledText.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyledText
#define _H_JXStyledText

#include <jx-af/jcore/JStyledText.h>

class JFontManager;

class JXStyledText : public JStyledText
{
public:

	JXStyledText(const bool useMultipleUndo, const bool pasteStyledText,
				 JFontManager* fontManager);
	JXStyledText(const JXStyledText& source);

	virtual ~JXStyledText();

protected:

	virtual bool	NeedsToAdjustFontToDisplayGlyphs(const JString& text, const JRunArray<JFont>& style) const;
	virtual bool	AdjustFontToDisplayGlyphs(const TextRange& range,
												  const JString& text,
												  JRunArray<JFont>* style);

private:

	JFontManager*	itsFontManager;		// can be nullptr
};

#endif
