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

	TextEditor(JStyledText* text, const JBoolean breakCROnly, const JCoordinate width);

	virtual ~TextEditor();

	virtual JBoolean	TEHasSearchText() const override;

protected:

	virtual void		TERefresh() override;
	virtual void		TERefreshRect(const JRect& rect) override;
	virtual void		TERedraw() override;
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) override;
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay) override;
	virtual JBoolean	TEScrollForDrag(const JPoint& pt) override;
	virtual JBoolean	TEScrollForDND(const JPoint& pt) override;
	virtual void		TESetVertScrollStep(const JCoordinate vStep) override;
	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) override;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual JBoolean	TEBeginDND() override;
	virtual void		TEPasteDropData() override;
	virtual void		TECaretShouldBlink(const JBoolean blink) override;

private:

	JCoordinate	itsWidth, itsHeight;

	JString*			itsClipText;
	JRunArray<JFont>*	itsClipStyle;

private:

	// not allowed

	TextEditor(const TextEditor& source);
	const TextEditor& operator=(const TextEditor& source);
};

#endif
