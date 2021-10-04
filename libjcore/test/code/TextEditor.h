/******************************************************************************
 TextEditor.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TextEditor
#define _H_TextEditor

#include <jx-af/jcore/JTextEditor.h>

class TextEditor : public JTextEditor
{
public:

	TextEditor(JStyledText* text, const bool breakCROnly, const JCoordinate width);

	virtual ~TextEditor();

	JCoordinate	GetWidth() const;
	JCoordinate	GetHeight() const;

	bool	TEHasSearchText() const override;
	void				SetHasSearchText(const bool has);

	// expose protected functionality

	void	SetBoundsWidth(const JCoordinate width);
	void	SetAllFontNameAndSize(const JString& name, const JSize size);

	JIndex	GetLineCharStart(const JIndex lineIndex) const;
	JIndex	GetLineCharEnd(const JIndex lineIndex) const;
	JIndex	GetLineCharLength(const JIndex lineIndex) const;

	JIndex		CalcCaretCharLocation(const JPoint& pt) const;
	bool	TestPointInSelection(const JPoint& pt) const;
	void		TestMoveCaretVert(const JInteger deltaLines);

	void	Draw();

	void	TestReplaceSelection(const JStringMatch& match, const JString& replaceStr,
								 JInterpolate* interpolator, const bool preserveCase);

	void	Activate();
	void	CheckCmdStatus(const JArray<bool>& expected) const;

	void	GetDoubleClickSelection(const JStyledText::TextIndex& charIndex,
									const bool partialWord,
									const bool dragging,
									JStyledText::TextRange* range);

	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
							   const CaretMotion motion, const bool deleteToTabStop);

protected:

	void		TERefresh() override;
	void		TERefreshRect(const JRect& rect) override;
	void		TERedraw() override;
	void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) override;
	bool	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	bool	TEScrollToRect(const JRect& rect,
									   const bool centerInDisplay) override;
	bool	TEScrollForDrag(const JPoint& pt) override;
	bool	TEScrollForDND(const JPoint& pt) override;
	void		TESetVertScrollStep(const JCoordinate vStep) override;
	void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	bool	TEBeginDND() override;
	void		TEPasteDropData() override;
	void		TECaretShouldBlink(const bool blink) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JCoordinate	itsWidth, itsHeight;

	mutable JString*			itsClipText;
	mutable JRunArray<JFont>*	itsClipStyle;

	bool	itsHasSearchTextFlag;

private:

	// not allowed

	TextEditor(const TextEditor& source);
	const TextEditor& operator=(const TextEditor& source);
};


/******************************************************************************
 Dimensions

 ******************************************************************************/

inline JCoordinate
TextEditor::GetWidth()
	const
{
	return itsWidth;
}

inline JCoordinate
TextEditor::GetHeight()
	const
{
	return itsHeight;
}

/******************************************************************************
 SetAllFontNameAndSize

 ******************************************************************************/

inline void
TextEditor::SetAllFontNameAndSize
	(
	const JString&	name,
	const JSize		size
	)
{
	JTextEditor::SetAllFontNameAndSize(name, size, 36, true, false);
}

/******************************************************************************
 GetLineCharStart

 ******************************************************************************/

inline JIndex
TextEditor::GetLineCharStart
	(
	const JIndex lineIndex
	)
	const
{
	return GetLineStart(lineIndex).charIndex;
}

/******************************************************************************
 GetLineCharEnd

	Returns the last character on the specified line.

 ******************************************************************************/

inline JIndex
TextEditor::GetLineCharEnd
	(
	const JIndex lineIndex
	)
	const
{
	return GetLineEnd(lineIndex).charIndex;
}

/******************************************************************************
 GetLineCharLength

 ******************************************************************************/

inline JSize
TextEditor::GetLineCharLength
	(
	const JIndex lineIndex
	)
	const
{
	return GetLineLength(lineIndex).charCount;
}

/******************************************************************************
 CalcCaretCharLocation

 ******************************************************************************/

inline JIndex
TextEditor::CalcCaretCharLocation
	(
	const JPoint& origPt
	)
	const
{
	JPoint pt = origPt;
	pt.x     -= TEGetLeftMarginWidth();
	return CalcCaretLocation(pt).location.charIndex;
}

/******************************************************************************
 TestPointInSelection

 ******************************************************************************/

inline bool
TextEditor::TestPointInSelection
	(
	const JPoint& pt
	)
	const
{
	return PointInSelection(pt);
}

/******************************************************************************
 TestMoveCaretVert

 ******************************************************************************/

inline void
TextEditor::TestMoveCaretVert
	(
	const JInteger deltaLines
	)
{
	MoveCaretVert(deltaLines);
}

#endif
