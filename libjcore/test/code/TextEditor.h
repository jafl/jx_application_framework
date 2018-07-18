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

	JCoordinate	GetWidth() const;
	JCoordinate	GetHeight() const;

	virtual JBoolean	TEHasSearchText() const override;
	void				SetHasSearchText(const JBoolean has);

	// expose protected functionality

	void	SetBoundsWidth(const JCoordinate width);
	void	SetAllFontNameAndSize(const JString& name, const JSize size);

	JIndex	GetLineCharStart(const JIndex lineIndex) const;
	JIndex	GetLineCharEnd(const JIndex lineIndex) const;
	JIndex	GetLineCharLength(const JIndex lineIndex) const;

	JIndex		CalcCaretCharLocation(const JPoint& pt) const;
	JBoolean	TestPointInSelection(const JPoint& pt) const;
	void		TestMoveCaretVert(const JInteger deltaLines);

	void	Draw();

	void	TestReplaceSelection(const JStringMatch& match, const JString& replaceStr,
								 JInterpolate* interpolator, const JBoolean preserveCase);

	void	Activate();
	void	CheckCmdStatus(const JRunArray<JBoolean>& expected) const;

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
	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual JBoolean	TEBeginDND() override;
	virtual void		TEPasteDropData() override;
	virtual void		TECaretShouldBlink(const JBoolean blink) override;

private:

	JCoordinate	itsWidth, itsHeight;

	mutable JString*			itsClipText;
	mutable JRunArray<JFont>*	itsClipStyle;

	JBoolean	itsHasSearchTextFlag;

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
	JTextEditor::SetAllFontNameAndSize(name, size, 36, kJTrue, kJTrue);
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

inline JBoolean
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
