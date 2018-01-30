/******************************************************************************
 TextEditor.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TextEditor
#define _H_TextEditor

#include <JTextEditor.h>

class TextEditor : public JTextEditor
{
public:

	TextEditor();

	virtual ~TextEditor();

	virtual JBoolean	TEHasSearchText() const;

	// expose protected functionality for testing

	JIndex	GetWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphEnd(const JIndex charIndex, const JIndex byteIndex) const;

protected:

	virtual void		TERefresh();
	virtual void		TERefreshRect(const JRect& rect);
	virtual void		TERedraw();
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY);
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay);
	virtual JBoolean	TEScrollForDrag(const JPoint& pt);
	virtual JBoolean	TEScrollForDND(const JPoint& pt);
	virtual void		TESetVertScrollStep(const JCoordinate vStep);
	virtual void		TEClipboardChanged();
	virtual JBoolean	TEGetExternalClipboard(JString* text, JRunArray<JFont>* style) const;
	virtual void		TEDisplayBusyCursor() const;
	virtual JBoolean	TEBeginDND();
	virtual void		TEPasteDropData();
	virtual void		TECaretShouldBlink(const JBoolean blink);

private:

	JString*			itsClipText;
	JRunArray<JFont>*	itsClipStyle;

private:

	// not allowed

	TextEditor(const TextEditor& source);
	const TextEditor& operator=(const TextEditor& source);
};

#endif
