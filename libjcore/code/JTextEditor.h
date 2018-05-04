/******************************************************************************
 JTextEditor.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextEditor
#define _H_JTextEditor

#include <JStyledTextBuffer.h>
#include <JRect.h>
#include <JInterpolate.h>

class JTEKeyHandler;
class JStringMatch;
class JPainter;
class JPagePrinter;

class JTextEditor : virtual public JBroadcaster
{
public:

	enum Type
	{
		kFullEditor,
		kSelectableText,
		kStaticText
	};

	enum CmdIndex
	{
		kSeparatorCmd = 1,	// always disabled, use if menu separator is separate item

		kUndoCmd,
		kRedoCmd,
		kCutCmd,
		kCopyCmd,
		kPasteCmd,
		kDeleteSelCmd,
		kSelectAllCmd,
		kCheckSpellingCmd,
		kCheckSpellingSelCmd,
		kCleanRightMarginCmd,
		kCoerceRightMarginCmd,
		kShiftSelLeftCmd,
		kShiftSelRightCmd,
		kForceShiftSelLeftCmd,
		kShowWhitespaceCmd,
		kCleanAllWhitespaceCmd,
		kCleanWhitespaceSelCmd,
		kCleanAllWSAlignCmd,
		kCleanWSAlignSelCmd,
		kToggleReadOnlyCmd,

		kFindDialogCmd,
		kFindPreviousCmd,
		kFindNextCmd,
		kEnterSearchTextCmd,
		kEnterReplaceTextCmd,
		kFindSelectionBackwardCmd,
		kFindSelectionForwardCmd,
		kFindClipboardBackwardCmd,
		kFindClipboardForwardCmd,
		kReplaceSelectionCmd,
		kReplaceFindPrevCmd,
		kReplaceFindNextCmd,
		kReplaceAllCmd,
		kReplaceAllInSelectionCmd,	// = kCmdCount

		kCmdCount = kReplaceAllInSelectionCmd
	};

	enum CaretMode
	{
		kLineCaret,
		kBlockCaret
	};

	enum CaretMotion
	{
		kMoveByCharacter,
		kMoveByPartialWord,
		kMoveByWord,
		kMoveByLine
	};

	enum
	{
		kMinLeftMarginWidth = 2
	};

public:

	JTextEditor(const Type type, JStyledTextBuffer* buffer, const JBoolean ownsBuffer,
				const JBoolean breakCROnly,
				const JColorID caretColor, const JColorID selectionColor,
				const JColorID outlineColor, const JColorID wsColor,
				const JCoordinate width);

	virtual ~JTextEditor();

	Type		GetType() const;
	JBoolean	IsReadOnly() const;

	JBoolean	WillBreakCROnly() const;
	void		SetBreakCROnly(const JBoolean breakCROnly);

	JStyledTextBuffer*			GetBuffer();
	const JStyledTextBuffer&	GetBuffer() const;

	JStringMatch	SearchForward(const JRegex& regex, const JBoolean entireWord,
								  const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SearchBackward(const JRegex& regex, const JBoolean entireWord,
								   const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SelectionMatches(const JRegex& regex, const JBoolean entireWord);

	JBoolean	ReplaceAll(const JRegex& regex, const JBoolean entireWord,
						   const JString& replaceStr, const JBoolean replaceIsRegex,
						   const JBoolean preserveCase,
						   const JBoolean restrictToSelection = kJFalse);

	JBoolean	SearchForward(const JStyledTextBuffer::FontMatch& match,
							  const JBoolean wrapSearch, JBoolean* wrapped);
	JBoolean	SearchBackward(const JStyledTextBuffer::FontMatch& match,
							   const JBoolean wrapSearch, JBoolean* wrapped);

	virtual JBoolean	TEHasSearchText() const = 0;

	JBoolean	GetCaretLocation(JIndex* charIndex) const;
	void		SetCaretLocation(const JIndex charIndex);

	JBoolean	HasSelection() const;
	JBoolean	GetSelection(JCharacterRange* range) const;
	JBoolean	GetSelection(JString* text) const;
	JBoolean	GetSelection(JString* text, JRunArray<JFont>* style) const;
/* don't call internally
	void		SetSelection(const JIndex startIndex, const JIndex endIndex,
							 const JBoolean needCaretBcast = kJTrue);
	void		SetSelection(const JCharacterRange& range,
							 const JBoolean needCaretBcast = kJTrue);
*/
	JBoolean	TEScrollToSelection(const JBoolean centerInDisplay);
	void		TEGetDoubleClickSelection(const JIndex charIndex,
										  const JBoolean partialWord,
										  const JBoolean dragging, JCharacterRange* range);

	void		GoToBeginningOfLine();
	void		GoToEndOfLine();

	JFont	GetCurrentFont() const;

	void	SetCurrentFontName(const JString& name);
	void	SetCurrentFontSize(const JSize size);
	void	SetCurrentFontBold(const JBoolean bold);
	void	SetCurrentFontItalic(const JBoolean italic);
	void	SetCurrentFontUnderline(const JSize count);
	void	SetCurrentFontStrike(const JBoolean strike);
	void	SetCurrentFontColor(const JColorID color);
	void	SetCurrentFontStyle(const JFontStyle& style);
	void	SetCurrentFont(const JFont& font);

	JCoordinate	GetDefaultTabWidth() const;
	void		SetDefaultTabWidth(const JCoordinate width);
	void		TabSelectionLeft(const JSize tabCount = 1,
								 const JBoolean force = kJFalse);
	void		TabSelectionRight(const JSize tabCount = 1);

	void	CleanAllWhitespace(const JBoolean align);
	void	CleanSelectedWhitespace(const JBoolean align);
	void	CleanWhitespace(const JCharacterRange& range, const JBoolean align);
	void	AnalyzeWhitespace(JSize* tabWidth);

	JBoolean	WillAutoIndent() const;
	void		ShouldAutoIndent(const JBoolean indent);

	JBoolean	WillShowWhitespace() const;
	void		ShouldShowWhitespace(const JBoolean show);

	JBoolean	WillMoveToFrontOfText() const;
	void		ShouldMoveToFrontOfText(const JBoolean moveToFront);

	JSize		GetLineCount() const;
	JIndex		GetLineForChar(const JIndex charIndex) const;
	void		GoToLine(const JIndex lineIndex);
	void		SelectLine(const JIndex lineIndex);
	JIndex		GetLineLength(const JIndex lineIndex) const;
	JCoordinate	GetLineTop(const JIndex lineIndex) const;
	JCoordinate	GetLineBottom(const JIndex lineIndex) const;
	JSize		GetLineHeight(const JIndex lineIndex) const;
	JIndex		CRLineIndexToVisualLineIndex(const JIndex crLineIndex) const;
	JIndex		VisualLineIndexToCRLineIndex(const JIndex visualLineIndex) const;

	JCoordinate	GetCharLeft(const JIndex charIndex) const;
	JCoordinate	GetCharRight(const JIndex charIndex) const;

	JIndex		GetColumnForChar(const JIndex charIndex) const;
	void		GoToColumn(const JIndex lineIndex, const JIndex columnIndex);

	void	DeleteSelection();
	void	SelectAll();

	void	Cut();
	void	Copy();
	void	Paste();															// clipboard
	void	Paste(const JString& text, const JRunArray<JFont>* style = NULL);	// other source

	JBoolean	GetClipboard(JString* text, JRunArray<JFont>* style = NULL) const;

	void	Paginate(const JCoordinate pageHeight,
					 JArray<JCoordinate>* breakpts) const;
	void	Print(JPagePrinter& p);

	JCharacterInWordFn	GetCharacterInWordFunction() const;
	void				SetCharacterInWordFunction(JCharacterInWordFn f);

	JTEKeyHandler*	GetKeyHandler() const;
	void			SetKeyHandler(JTEKeyHandler* handler);

	JBoolean	AllowsDragAndDrop() const;
	void		ShouldAllowDragAndDrop(const JBoolean allow);

	static JBoolean	WillCopyWhenSelect();
	static void		ShouldCopyWhenSelect(const JBoolean copy);

	JCoordinate	TEGetMinPreferredGUIWidth() const;

	JCoordinate	TEGetLeftMarginWidth() const;
	void		TESetLeftMarginWidth(const JCoordinate width);

	JColorID	GetCaretColor() const;
	void		SetCaretColor(const JColorID color);
	JColorID	GetSelectionColor() const;
	void		SetSelectionColor(const JColorID color);
	JColorID	GetSelectionOutlineColor() const;
	void		SetSelectionOutlineColor(const JColorID color);
	JColorID	GetWhitespaceColor() const;
	void		SetWhitespaceColor(const JColorID color);

	JBoolean	WillBroadcastCaretLocationChanged() const;
	void		ShouldBroadcastCaretLocationChanged(const JBoolean broadcast);

	CaretMode	GetCaretMode() const;
	void		SetCaretMode(const CaretMode mode);

public:		// ought to be protected

	struct CaretLocation
	{
		JStyledTextBuffer::TextIndex location;	// caret is in front of this location in the buffer
		JIndex lineIndex;						// caret is on this line of text

		CaretLocation()
			:
			lineIndex(0)
			{ };

		CaretLocation(const JStyledTextBuffer::TextIndex& loc, const JIndex line)
			:
			location(loc),
			lineIndex(line)
			{ };
	};

	struct LineGeometry
	{
		JCoordinate height;		// height of line
		JCoordinate ascent;		// offset to font baseline

		LineGeometry()
			:
			height(0),
			ascent(0)
			{ };

		LineGeometry(const JCoordinate aHeight, const JCoordinate anAscent)
			:
			height(aHeight),
			ascent(anAscent)
			{ };
	};

protected:

	JBoolean	TEIsActive() const;
	void		TEActivate();
	void		TEDeactivate();

	JBoolean	TESelectionIsActive() const;
	void		TEActivateSelection();
	void		TEDeactivateSelection();

	JBoolean	TECaretIsVisible() const;
	void		TEShowCaret();
	void		TEHideCaret();

	void	SetType(const Type type);

	void				RecalcAll();
	JArray<JBoolean>	GetCmdStatus(JString* crmActionText, JString* crm2ActionText,
									 JBoolean* isReadOnly) const;

	void			TEDraw(JPainter& p, const JRect& rect);
	virtual void	TEDrawInMargin(JPainter& p, const JPoint& topLeft,
								   const LineGeometry& geom, const JIndex lineIndex);

	JBoolean	TEWillDragAndDrop(const JPoint& pt, const JBoolean extendSelection,
								  const JBoolean dropCopy) const;
	void		TEHandleMouseDown(const JPoint& pt, const JSize clickCount,
								  const JBoolean extendSelection,
								  const JBoolean partialWord);
	void		TEHandleMouseDrag(const JPoint& pt);
	void		TEHandleMouseUp(const JBoolean dropCopy);
	JBoolean	TEHitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual JBoolean	TEBeginDND() = 0;
	void				TEDNDFinished();
	void				TEHandleDNDEnter();
	void				TEHandleDNDHere(const JPoint& pt, const JBoolean dropOnSelf);
	void				TEHandleDNDLeave();
	void				TEHandleDNDDrop(const JPoint& pt, const JBoolean dropOnSelf,
										const JBoolean dropCopy);
	virtual void		TEPasteDropData() = 0;

	JBoolean	TEIsDragging() const;
	JBoolean	TEHandleKeyPress(const JUtf8Character& key, const JBoolean selectText,
								 const CaretMotion motion, const JBoolean deleteToTabStop);

	JCoordinate	TEGetBoundsWidth() const;
	void		TESetBoundsWidth(const JCoordinate width);

	virtual void		TERefresh() = 0;
	virtual void		TERefreshRect(const JRect& rect) = 0;
	void				TERefreshLines(const JIndex first, const JIndex last);
	virtual void		TEUpdateDisplay();
	virtual void		TERedraw() = 0;
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) = 0;
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const = 0;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay) = 0;
	virtual JBoolean	TEScrollForDrag(const JPoint& pt) = 0;
	virtual JBoolean	TEScrollForDND(const JPoint& pt) = 0;
	virtual void		TESetVertScrollStep(const JCoordinate vStep) = 0;
	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) = 0;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const = 0;
	virtual void		TECaretShouldBlink(const JBoolean blink) = 0;

	virtual JCoordinate	GetTabWidth(const JStyledTextBuffer::TextIndex& charIndex, const JCoordinate x) const;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	CRMConvertTab(JString* charBuffer, JSize* charCount,
								  const JSize currentLineWidth) const;


	JBoolean		GetCaretLocation(CaretLocation* caretLoc) const;
	CaretLocation	CalcCaretLocation(const JPoint& pt) const;
	JBoolean		PointInSelection(const JPoint& pt) const;
	void			MoveCaretVert(const JInteger deltaLines);
	JIndex			GetColumnForChar(const CaretLocation& caretLoc) const;

	JStyledTextBuffer::TextIndex	GetInsertionIndex() const;
	JStyledTextBuffer::TextIndex	GetLineStart(const JIndex lineIndex) const;
	JStyledTextBuffer::TextIndex	GetLineEnd(const JIndex lineIndex) const;

	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const JCoordinate tabWidth,
								  const JBoolean breakCROnly,
								  const JBoolean clearUndo);

	void	ReplaceSelection(const JStringMatch& match,
							 const JString& replaceStr,
							 const JBoolean replaceIsRegex,
							 const JBoolean preserveCase);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kSelectPartialWordDrag,
		kSelectWordDrag,
		kSelectLineDrag,
		kDragAndDrop
	};

private:

	Type				itsType;
	JStyledTextBuffer*	itsBuffer;
	const JBoolean		itsOwnsBufferFlag;

	JBoolean	itsActiveFlag;
	JBoolean	itsBreakCROnlyFlag;			// kJFalse => break line at whitespace
	JBoolean	itsPerformDNDFlag;			// kJTrue => drag-and-drop enabled
	JBoolean	itsAutoIndentFlag;			// kJTrue => auto-indent after newline enabled
	JBoolean	itsMoveToFrontOfTextFlag;	// kJTrue => left arrow w/ moveEOL puts caret after whitespace
	JBoolean	itsBcastLocChangedFlag;		// kJTrue => broadcast CaretLocationChanged instead of CaretLineChanged
	JBoolean	itsIsPrintingFlag;			// kJTrue => stack threads through Print()
	JBoolean	itsDrawWhitespaceFlag;		// kJTrue => show tabs, spaces, newlines
	CaretMode	itsCaretMode;

	JColorID	itsCaretColor;
	JColorID	itsSelectionColor;
	JColorID	itsSelectionOutlineColor;
	JColorID	itsWhitespaceColor;

	JCoordinate	itsWidth;			// pixels -- width of widest line
	JCoordinate	itsHeight;			// pixels
	JCoordinate	itsGUIWidth;		// pixels -- available width of widget aperture
	JCoordinate	itsLeftMarginWidth;	// pixels
	JCoordinate	itsDefTabWidth;		// pixels
	JCoordinate	itsMaxWordWidth;	// pixels -- widest single word; only if word wrap

	JArray<JStyledTextBuffer::TextIndex>*	itsLineStarts;	// index of first character on each line
	JRunArray<LineGeometry>*				itsLineGeom;	// geometry of each line

	JInterpolate	itsInterpolator;
	JTEKeyHandler*	itsKeyHandler;

	// information for Recalc

	JSize	itsPrevBufLength;	// buffer length after last Recalc

	// used while active

	JBoolean		itsSelActiveFlag;		// kJTrue => draw solid selection
	JBoolean		itsCaretVisibleFlag;	// kJTrue => draw caret
	CaretLocation	itsCaretLoc;			// insertion point is -at- this character; do not set directly - call SetCaretLocation()
	JCoordinate		itsCaretX;				// horizontal location used by MoveCaretVert()
	JFont			itsInsertionFont;		// style for characters that user types

	JStyledTextBuffer::TextRange	itsSelection;		// caret is visible if this is empty

	// used during drag

	DragType						itsDragType;
	DragType						itsPrevDragType;	// so extend selection will maintain drag type
	JPoint							itsStartPt;
	JPoint							itsPrevPt;
	JStyledTextBuffer::TextIndex	itsSelectionPivot;	// insertion point about which to pivot selection
	JCharacterRange					itsWordSelPivot;	// range of characters to keep selected
	JIndex							itsLineSelPivot;	// line about which to pivot selection
	CaretLocation					itsDropLoc;			// insertion point at which to drop the dragged text
	JBoolean						itsIsDragSourceFlag;// kJTrue => is dragging even if itsDragType == kInvalidDrag

	// global

	static JBoolean	theCopyWhenSelectFlag;	// kJTrue => SetSelection() calls Copy()

private:

	void		Recalc(const JStyledTextBuffer::TextRange& recalcRange,
					   const JStyledTextBuffer::TextRange& redrawRange,
					   const JBoolean deletion);
	void		Recalc1(const JSize bufLength, const CaretLocation& caretLoc,
						const JSize minCharCount, JCoordinate* maxLineWidth,
						JIndex* firstLineIndex, JIndex* lastLineIndex);
	JSize		RecalcLine(const JSize bufLength,
						   const JStyledTextBuffer::TextIndex& firstIndex,
						   const JIndex lineIndex, JCoordinate* lineWidth,
						   JIndex* runIndex, JIndex* firstInRun);
	JBoolean	LocateNextWhitespace(const JSize bufLength,
									 JStyledTextBuffer::TextIndex* startIndex) const;
	JSize		GetSubwordForLine(const JSize bufLength, const JIndex lineIndex,
								  const JStyledTextBuffer::TextIndex& startIndex,
								  JCoordinate* lineWidth) const;
	JSize		IncludeWhitespaceOnLine(const JSize bufLength,
										const JStyledTextBuffer::TextIndex& startIndex,
										JCoordinate* lineWidth, JBoolean* endOfLine,
										JIndex* runIndex, JIndex* firstInRun) const;
	JBoolean	NoPrevWhitespaceOnLine(const JString& str, const CaretLocation& startLoc) const;

	void	TEDrawText(JPainter& p, const JRect& rect);
	void	TEDrawLine(JPainter& p, const JCoordinate top, const LineGeometry& geom,
					   const JIndex lineIndex, JIndex* runIndex, JIndex* firstInRun);
	void	TEDrawSelection(JPainter& p, const JRect& rect, const JIndex startVisLine,
							const JCoordinate startVisLineTop);
	void	TEDrawCaret(JPainter& p, const CaretLocation& caretLoc);

	JFont	CalcInsertionFont(const JStyledTextBuffer::TextIndex& index) const;
	JFont	CalcInsertionFont(const JStringIterator& buffer,
							  const JRunArray<JFont>& styles) const;
	void	DropSelection(const JStyledTextBuffer::TextIndex& dropLoc, const JBoolean dropCopy);

	JStyledTextBuffer::TextCount	PrivatePaste(const JString& text, const JRunArray<JFont>* style);
	JStyledTextBuffer::TextCount	InsertText(const JStyledTextBuffer::TextIndex& index, const JString& text,
											   const JRunArray<JFont>* style = NULL);

	void			SetCaretLocation(const JStyledTextBuffer::TextIndex& caretLoc);
	void			SetCaretLocation(const CaretLocation& caretLoc);
	CaretLocation	CalcCaretLocation(const JStyledTextBuffer::TextIndex& index) const;
	JIndex			CalcLineIndex(const JCoordinate y, JCoordinate* lineTop) const;
	JBoolean		TEScrollTo(const CaretLocation& caretLoc);
	JRect			CalcCaretRect(const CaretLocation& caretLoc) const;
	void			TERefreshCaret(const CaretLocation& caretLoc);

	JBoolean	GetSelection(JStyledTextBuffer::TextRange* range) const;
	void		SetSelection(const JStyledTextBuffer::TextRange& range,
							 const JBoolean needCaretBcast = kJTrue);

	JCoordinate	GetCharLeft(const CaretLocation& charLoc) const;
	JCoordinate	GetCharRight(const CaretLocation& charLoc) const;
	JCoordinate	GetCharWidth(const CaretLocation& charLoc) const;
	JCoordinate	GetStringWidth(const JStyledTextBuffer::TextIndex& startIndex,
							   const JStyledTextBuffer::TextIndex& endIndex) const;
	JCoordinate	GetStringWidth(const JStyledTextBuffer::TextIndex& startIndex,
							   const JStyledTextBuffer::TextIndex& endIndex,
							   JIndex* runIndex, JIndex* firstInRun) const;

	JStyledTextBuffer::TextIndex	CharToTextIndex(const JIndex charIndex) const;
	JStyledTextBuffer::TextRange	CharToTextRange(const JCharacterRange& charRange) const;
	JIndex							GetLineForByte(const JIndex byteIndex) const;

	void	PrivateSetBreakCROnly(const JBoolean breakCROnly);
	void	TEGUIWidthChanged();

	JCoordinate	GetEWNHeight() const;

	JRect	CalcLocalDNDRect(const JPoint& pt) const;

	void	InsertKeyPress(const JUtf8Character& key);
	void	BackwardDelete(const JBoolean deleteToTabStop,
						   JString* returnText = NULL, JRunArray<JFont>* returnStyle = NULL);
	void	ForwardDelete(const JBoolean deleteToTabStop,
						  JString* returnText = NULL, JRunArray<JFont>* returnStyle = NULL);

	void	AutoIndent(JTEUndoTyping* typingUndo);
	void	InsertSpacesForTab();

	JBoolean	LocateTab(const JStyledTextBuffer::TextIndex& startIndex,
						  const JStyledTextBuffer::TextIndex& endIndex,
						  JStyledTextBuffer::TextIndex* tabIndex) const;

	void		ReplaceRange(JStringIterator* buffer, JRunArray<JFont>* styles,
							 const JStringMatch& match,
							 const JString& replaceStr,
							 const JBoolean replaceIsRegex,
							 const JBoolean preserveCase);

	void		BroadcastCaretMessages(const CaretLocation& caretLoc,
									   const JBoolean lineChanged);

	static JInteger	GetLineHeight(const LineGeometry& data);

	// not allowed

	JTextEditor(const JTextEditor& source);
	const JTextEditor& operator=(const JTextEditor& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTypeChanged;
	static const JUtf8Byte* kCaretLineChanged;
	static const JUtf8Byte* kCaretLocationChanged;

	class TypeChanged : public JBroadcaster::Message
	{
	public:

		TypeChanged(const Type type)
			:
			JBroadcaster::Message(kTypeChanged),
			itsType(type)
			{ };

		Type
		GetNewType() const
		{
			return itsType;
		};

	private:

		Type	itsType;
	};

	class CaretLineChanged : public JBroadcaster::Message
	{
	public:

		CaretLineChanged(const JIndex lineIndex)
			:
			JBroadcaster::Message(kCaretLineChanged),
			itsLineIndex(lineIndex)
			{ };

		JIndex
		GetLineIndex() const
		{
			return itsLineIndex;
		};

	private:

		JIndex itsLineIndex;
	};

	class CaretLocationChanged : public JBroadcaster::Message
	{
	public:

		CaretLocationChanged(const JIndex lineIndex, const JIndex columnIndex)
			:
			JBroadcaster::Message(kCaretLocationChanged),
			itsLineIndex(lineIndex),
			itsColumnIndex(columnIndex)
			{ };

		JIndex
		GetLineIndex() const
		{
			return itsLineIndex;
		};

		JIndex
		GetColumnIndex() const
		{
			return itsColumnIndex;
		};

	private:

		JIndex itsLineIndex, itsColumnIndex;
	};
};


/******************************************************************************
 GetType

 ******************************************************************************/

inline JTextEditor::Type
JTextEditor::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 IsReadOnly

 ******************************************************************************/

inline JBoolean
JTextEditor::IsReadOnly()
	const
{
	return JConvertToBoolean( itsType != kFullEditor );
}

/******************************************************************************
 TEIsActive (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor::TEIsActive()
	const
{
	return itsActiveFlag;
}

/******************************************************************************
 TEActivate (protected)

 ******************************************************************************/

inline void
JTextEditor::TEActivate()
{
	if (!itsActiveFlag)
		{
		itsActiveFlag = kJTrue;
		TERefresh();
		}
}

/******************************************************************************
 TEDeactivate (protected)

 ******************************************************************************/

inline void
JTextEditor::TEDeactivate()
{
	if (itsActiveFlag)
		{
		itsActiveFlag = kJFalse;
		TERefresh();
		}
}

/******************************************************************************
 TESelectionIsActive (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor::TESelectionIsActive()
	const
{
	return itsSelActiveFlag;
}

/******************************************************************************
 TEActivateSelection (protected)

 ******************************************************************************/

inline void
JTextEditor::TEActivateSelection()
{
	if (!itsSelActiveFlag)
		{
		itsSelActiveFlag = kJTrue;
		TERefresh();
		}
}

/******************************************************************************
 TEDeactivateSelection (protected)

 ******************************************************************************/

inline void
JTextEditor::TEDeactivateSelection()
{
	if (itsSelActiveFlag)
		{
		itsSelActiveFlag = kJFalse;
		TERefresh();
		}
}

/******************************************************************************
 TECaretIsVisible (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor::TECaretIsVisible()
	const
{
	return itsCaretVisibleFlag;
}

/******************************************************************************
 TEShowCaret (protected)

 ******************************************************************************/

inline void
JTextEditor::TEShowCaret()
{
	if (!itsCaretVisibleFlag)
		{
		itsCaretVisibleFlag = kJTrue;
		TERefreshCaret(itsCaretLoc);
		}
}

/******************************************************************************
 TEHideCaret (protected)

 ******************************************************************************/

inline void
JTextEditor::TEHideCaret()
{
	if (itsCaretVisibleFlag)
		{
		itsCaretVisibleFlag = kJFalse;
		TERefreshCaret(itsCaretLoc);
		}
}

/******************************************************************************
 GetBuffer

 ******************************************************************************/

inline JStyledTextBuffer*
JTextEditor::GetBuffer()
{
	return itsBuffer;
}

inline const JStyledTextBuffer&
JTextEditor::GetBuffer()
	const
{
	return *itsBuffer;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline JBoolean
JTextEditor::HasSelection()
	const
{
	return !itsSelection.IsEmpty();
}

inline JBoolean
JTextEditor::GetSelection
	(
	JCharacterRange* range
	)
	const
{
	*range = itsSelection.charRange;
	return !itsSelection.IsEmpty();
}
/*
inline void
JTextEditor::SetSelection
	(
	const JCharacterRange&	range,
	const JBoolean			needCaretBcast
	)
{
	SetSelection(range.first, range.last, needCaretBcast);
}
*/

// protected

inline JBoolean
JTextEditor::GetSelection
	(
	JStyledTextBuffer::TextRange* range
	)
	const
{
	*range = itsSelection;
	return !itsSelection.IsEmpty();
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

inline void
JTextEditor::SelectAll()
{
	if (!itsBuffer->GetText().IsEmpty())
		{
		SetSelection(JStyledTextBuffer::TextRange(
			JCharacterRange(1, itsBuffer->GetText().GetCharacterCount()),
			JUtf8ByteRange( 1, itsBuffer->GetText().GetByteCount())));
		}
}

/******************************************************************************
 Default tab width

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetDefaultTabWidth()
	const
{
	return itsDefTabWidth;
}

inline void
JTextEditor::SetDefaultTabWidth
	(
	const JCoordinate width
	)
{
	if (width != itsDefTabWidth)
		{
		itsDefTabWidth = width;
		RecalcAll();
		}
}

/******************************************************************************
 Auto indenting

 ******************************************************************************/

inline JBoolean
JTextEditor::WillAutoIndent()
	const
{
	return itsAutoIndentFlag;
}

inline void
JTextEditor::ShouldAutoIndent
	(
	const JBoolean indent
	)
{
	itsAutoIndentFlag = indent;
}

/******************************************************************************
 CleanAllWhitespace

	Clean up the indentation whitespace and strip trailing whitespace.

 ******************************************************************************/

inline void
JTextEditor::CleanAllWhitespace
	(
	const JBoolean align
	)
{
	CleanWhitespace(JCharacterRange(1, itsBuffer->GetText().GetCharacterCount()), align);
}

/******************************************************************************
 CleanSelectedWhitespace

	Clean up the indentation whitespace and strip trailing whitespace in
	the selected range.

 ******************************************************************************/

inline void
JTextEditor::CleanSelectedWhitespace
	(
	const JBoolean align
	)
{
	JCharacterRange r;
	if (GetSelection(&r))
		{
		CleanWhitespace(r, align);
		}
}

/******************************************************************************
 Whitespace is visible

 ******************************************************************************/

inline JBoolean
JTextEditor::WillShowWhitespace()
	const
{
	return itsDrawWhitespaceFlag;
}

inline void
JTextEditor::ShouldShowWhitespace
	(
	const JBoolean show
	)
{
	if (show != itsDrawWhitespaceFlag)
		{
		itsDrawWhitespaceFlag = show;
		TERefresh();
		}
}

/******************************************************************************
 Move caret to front of text instead of front of line

 ******************************************************************************/

inline JBoolean
JTextEditor::WillMoveToFrontOfText()
	const
{
	return itsMoveToFrontOfTextFlag;
}

inline void
JTextEditor::ShouldMoveToFrontOfText
	(
	const JBoolean moveToFront
	)
{
	itsMoveToFrontOfTextFlag = moveToFront;
}

/******************************************************************************
 Automatically copy when select text (static)

	This is primarily useful under the X Window System.  It is static
	because all text should work the same way.

 ******************************************************************************/

inline JBoolean
JTextEditor::WillCopyWhenSelect()
{
	return theCopyWhenSelectFlag;
}

inline void
JTextEditor::ShouldCopyWhenSelect
	(
	const JBoolean copy
	)
{
	theCopyWhenSelectFlag = copy;
}

/******************************************************************************
 Key handler

 ******************************************************************************/

inline JTEKeyHandler*
JTextEditor::GetKeyHandler()
	const
{
	return itsKeyHandler;
}

/******************************************************************************
 Drag-And-Drop

 ******************************************************************************/

inline JBoolean
JTextEditor::AllowsDragAndDrop()
	const
{
	return itsPerformDNDFlag;
}

inline void
JTextEditor::ShouldAllowDragAndDrop
	(
	const JBoolean allow
	)
{
	itsPerformDNDFlag = allow;
}

/******************************************************************************
 GetCaretLocation

 ******************************************************************************/

inline JBoolean
JTextEditor::GetCaretLocation
	(
	JIndex* charIndex
	)
	const
{
	*charIndex = itsCaretLoc.location.charIndex;
	return itsSelection.IsEmpty();
}

// protected

inline JBoolean
JTextEditor::GetCaretLocation
	(
	CaretLocation* caretLoc
	)
	const
{
	*caretLoc = itsCaretLoc;
	return itsSelection.IsEmpty();
}

// private

void
JTextEditor::SetCaretLocation
	(
	const JStyledTextBuffer::TextIndex& index
	)
{
	SetCaretLocation(CalcCaretLocation(index));
}

/******************************************************************************
 TEGetBoundsWidth (protected)

 ******************************************************************************/

inline JCoordinate
JTextEditor::TEGetBoundsWidth()
	const
{
	return itsWidth;
}

/******************************************************************************
 TEGetLeftMarginWidth

 ******************************************************************************/

inline JCoordinate
JTextEditor::TEGetLeftMarginWidth()
	const
{
	return itsLeftMarginWidth;
}

/******************************************************************************
 WillBreakCROnly

 ******************************************************************************/

inline JBoolean
JTextEditor::WillBreakCROnly()
	const
{
	return itsBreakCROnlyFlag;
}

inline void
JTextEditor::SetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	if (breakCROnly != itsBreakCROnlyFlag)
		{
		PrivateSetBreakCROnly(breakCROnly);
		RecalcAll();
		}
}

/******************************************************************************
 GetLineCount

 ******************************************************************************/

inline JSize
JTextEditor::GetLineCount()
	const
{
	return itsLineStarts->GetElementCount();
}

/******************************************************************************
 GetLineStart (protected)

	Returns the first character on the specified line.

 ******************************************************************************/

inline JStyledTextBuffer::TextIndex
JTextEditor::GetLineStart
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineStarts->GetElement(lineIndex);
}

/******************************************************************************
 GetLineLength

	Returns the number of characters on the specified line.

 ******************************************************************************/

inline JSize
JTextEditor::GetLineLength
	(
	const JIndex lineIndex
	)
	const
{
	return (GetLineEnd(lineIndex).charIndex - GetLineStart(lineIndex).charIndex + 1);
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

inline JSize
JTextEditor::GetLineHeight
	(
	const JIndex lineIndex
	)
	const
{
	return (itsLineGeom->GetElement(lineIndex)).height;
}

/******************************************************************************
 GetLineBottom

	Returns the bottom of the specified line.

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetLineBottom
	(
	const JIndex lineIndex
	)
	const
{
	return (GetLineTop(lineIndex) + GetLineHeight(lineIndex) - 1);
}

/******************************************************************************
 GetCharLeft

	Returns the starting x coordinate of the specified character, including
	the left margin width

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetCharLeft
	(
	const JIndex charIndex
	)
	const
{
	return itsLeftMarginWidth +
			GetCharLeft(CalcCaretLocation(JStyledTextBuffer::TextIndex(charIndex, 0)));
}

/******************************************************************************
 GetCharRight

	Returns the ending x coordinate of the specified character, including
	the left margin width

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetCharRight
	(
	const JIndex charIndex
	)
	const
{
	return itsLeftMarginWidth +
		GetCharRight(CalcCaretLocation(JStyledTextBuffer::TextIndex(charIndex, 0)));
}

/******************************************************************************
 Broadcast CaretLocationChanged instead of CaretLineChanged

	Broadcasting both messages is redundant.

 ******************************************************************************/

inline JBoolean
JTextEditor::WillBroadcastCaretLocationChanged()
	const
{
	return itsBcastLocChangedFlag;
}

inline void
JTextEditor::ShouldBroadcastCaretLocationChanged
	(
	const JBoolean broadcast
	)
{
	itsBcastLocChangedFlag = broadcast;
}

/******************************************************************************
 Caret mode

 ******************************************************************************/

inline JTextEditor::CaretMode
JTextEditor::GetCaretMode()
	const
{
	return itsCaretMode;
}

inline void
JTextEditor::SetCaretMode
	(
	const CaretMode mode
	)
{
	itsCaretMode = mode;
}

/******************************************************************************
 Colors

 ******************************************************************************/

inline JColorID
JTextEditor::GetCaretColor()
	const
{
	return itsCaretColor;
}

inline void
JTextEditor::SetCaretColor
	(
	const JColorID color
	)
{
	if (color != itsCaretColor)
		{
		itsCaretColor = color;
		TERefreshCaret(itsCaretLoc);
		}
}

inline JColorID
JTextEditor::GetSelectionColor()
	const
{
	return itsSelectionColor;
}

inline void
JTextEditor::SetSelectionColor
	(
	const JColorID color
	)
{
	if (color != itsSelectionColor)
		{
		itsSelectionColor = color;
		if (!itsSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorID
JTextEditor::GetSelectionOutlineColor()
	const
{
	return itsSelectionOutlineColor;
}

inline void
JTextEditor::SetSelectionOutlineColor
	(
	const JColorID color
	)
{
	if (color != itsSelectionOutlineColor)
		{
		itsSelectionOutlineColor = color;
		if (!itsSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorID
JTextEditor::GetWhitespaceColor()
	const
{
	return itsWhitespaceColor;
}

inline void
JTextEditor::SetWhitespaceColor
	(
	const JColorID color
	)
{
	if (color != itsWhitespaceColor)
		{
		itsWhitespaceColor = color;
		TERefresh();
		}
}

/******************************************************************************
 GetCurrentFont

 ******************************************************************************/

inline JFont
JTextEditor::GetCurrentFont()
	const
{
	if (!itsSelection.IsEmpty())
		{
		return itsBuffer->GetStyles().GetElement(itsSelection.charRange.first);
		}
	else
		{
		return itsInsertionFont;
		}
}

/******************************************************************************
 Set current font

 ******************************************************************************/

inline void
JTextEditor::SetCurrentFontName
	(
	const JString& name
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontName(itsSelection, name, kJFalse);
		}
	else
		{
		itsInsertionFont.SetName(name);
		}
}

inline void
JTextEditor::SetCurrentFontSize
	(
	const JSize size
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontSize(itsSelection, size, kJFalse);
		}
	else
		{
		itsInsertionFont.SetSize(size);
		}
}

inline void
JTextEditor::SetCurrentFontBold
	(
	const JBoolean bold
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontBold(itsSelection, bold, kJFalse);
		}
	else
		{
		itsInsertionFont.SetBold(bold);
		}
}

inline void
JTextEditor::SetCurrentFontItalic
	(
	const JBoolean italic
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontItalic(itsSelection, italic, kJFalse);
		}
	else
		{
		itsInsertionFont.SetItalic(italic);
		}
}

inline void
JTextEditor::SetCurrentFontUnderline
	(
	const JSize count
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontUnderline(itsSelection, count, kJFalse);
		}
	else
		{
		itsInsertionFont.SetUnderlineCount(count);
		}
}

inline void
JTextEditor::SetCurrentFontStrike
	(
	const JBoolean strike
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontStrike(itsSelection, strike, kJFalse);
		}
	else
		{
		itsInsertionFont.SetStrike(strike);
		}
}

inline void
JTextEditor::SetCurrentFontColor
	(
	const JColorID color
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontColor(itsSelection, color, kJFalse);
		}
	else
		{
		itsInsertionFont.SetColor(color);
		}
}

inline void
JTextEditor::SetCurrentFontStyle
	(
	const JFontStyle& style
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFontStyle(itsSelection, style, kJFalse);
		}
	else
		{
		itsInsertionFont.SetStyle(style);
		}
}

inline void
JTextEditor::SetCurrentFont
	(
	const JFont& f
	)
{
	if (!itsSelection.IsEmpty())
		{
		itsBuffer->SetFont(itsSelection, f, kJFalse);
		}
	else
		{
		itsInsertionFont = f;
		}
}

/******************************************************************************
 Dragging (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor::TEIsDragging()
	const
{
	return JI2B( itsDragType != kInvalidDrag || itsIsDragSourceFlag );
}

inline void
JTextEditor::TEDNDFinished()
{
	itsIsDragSourceFlag = kJFalse;
}

/******************************************************************************
 CaretLocation operators

 ******************************************************************************/

inline int
operator==
	(
	const JTextEditor::CaretLocation& l1,
	const JTextEditor::CaretLocation& l2
	)
{
	return (l1.location.charIndex == l2.location.charIndex &&
			l1.location.byteIndex == l2.location.byteIndex);
}

inline int
operator!=
	(
	const JTextEditor::CaretLocation& l1,
	const JTextEditor::CaretLocation& l2
	)
{
	return (l1.location.charIndex != l2.location.charIndex ||
			l1.location.byteIndex != l2.location.byteIndex);
}

/******************************************************************************
 LineGeometry operators

 ******************************************************************************/

inline int
operator==
	(
	const JTextEditor::LineGeometry& g1,
	const JTextEditor::LineGeometry& g2
	)
{
	return (g1.height == g2.height && g1.ascent == g2.ascent);
}

inline int
operator!=
	(
	const JTextEditor::LineGeometry& g1,
	const JTextEditor::LineGeometry& g2
	)
{
	return !(g1 == g2);
}

#endif
