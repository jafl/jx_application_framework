/******************************************************************************
 JTextEditor.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextEditor
#define _H_JTextEditor

#include "jx-af/jcore/JStyledText.h"
#include "jx-af/jcore/JRect.h"

class JTEKeyHandler;
class JInterpolate;
class JFontManager;
class JStringMatch;
class JPainter;
class JPagePrinter;

class JTextEditor : virtual public JBroadcaster
{
	friend class JTEKeyHandler;
	friend class JTEDefaultKeyHandler;
	friend class JVIKeyHandler;

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

	JTextEditor(const Type type, JStyledText* text, const bool ownsText,
				JFontManager* fontManager, const bool breakCROnly,
				const JColorID caretColor, const JColorID selectionColor,
				const JColorID outlineColor, const JColorID wsColor,
				const JCoordinate width);

	JTextEditor(const JTextEditor& source, JStyledText* text);

	~JTextEditor() override;

	Type	GetType() const;
	bool	IsReadOnly() const;

	bool	WillBreakCROnly() const;
	void	SetBreakCROnly(const bool breakCROnly);

	JStyledText*		GetText();
	const JStyledText&	GetText() const;

	JStringMatch	SearchForward(const JRegex& regex, const bool entireWord,
								  const bool wrapSearch, bool* wrapped);
	JStringMatch	SearchBackward(const JRegex& regex, const bool entireWord,
								   const bool wrapSearch, bool* wrapped);
	JStringMatch	SelectionMatches(const JRegex& regex, const bool entireWord);

	bool	ReplaceAll(const JRegex& regex, const bool entireWord,
					   const JString& replaceStr, JInterpolate* interpolator,
					   const bool preserveCase,
					   const bool restrictToSelection = false);

	bool	SearchForward(std::function<bool(const JFont&)> match,
						  const bool wrapSearch, bool* wrapped);
	bool	SearchBackward(std::function<bool(const JFont&)> match,
						   const bool wrapSearch, bool* wrapped);

	virtual bool	TEHasSearchText() const = 0;

	JIndex				GetInsertionCharIndex() const;
	JStringIterator*	GetConstIteratorAtInsertionIndex() const;
	void				DisposeConstIterator(JStringIterator* iter) const;

	bool	GetCaretLocation(JIndex* charIndex) const;
	void	SetCaretLocation(const JIndex charIndex);
	void	SetCaretLocation(const JPoint& pt);

	bool	HasSelection() const;
	bool	GetSelection(JCharacterRange* range) const;
	bool	GetSelection(JString* text) const;
	bool	GetSelection(JString* text, JRunArray<JFont>* style) const;
	void	SetSelection(const JCharacterRange& range,
						 const bool needCaretBcast = true);

	bool	TEScrollToSelection(const bool centerInDisplay);

	void	GoToBeginningOfLine();
	void	GoToEndOfLine();

	JFont	GetCurrentFont() const;

	void	SetCurrentFontName(const JString& name);
	void	SetCurrentFontSize(const JSize size);
	void	SetCurrentFontBold(const bool bold);
	void	SetCurrentFontItalic(const bool italic);
	void	SetCurrentFontUnderline(const JSize count);
	void	SetCurrentFontStrike(const bool strike);
	void	SetCurrentFontColor(const JColorID color);
	void	SetCurrentFontStyle(const JFontStyle& style);
	void	SetCurrentFont(const JFont& font);

	JCoordinate	GetDefaultTabWidth() const;
	void		SetDefaultTabWidth(const JCoordinate width);
	void		TabSelectionLeft(const JSize tabCount = 1,
								 const bool force = false);
	void		TabSelectionRight(const JSize tabCount = 1);

	void	CleanAllWhitespace(const bool align);
	void	CleanSelectedWhitespace(const bool align);
	void	AnalyzeWhitespace(JSize* tabWidth);

	bool	WillShowWhitespace() const;
	void	ShouldShowWhitespace(const bool show);

	bool	WillAlwaysShowSelection() const;
	void	ShouldAlwaysShowSelection(const bool show);

	bool	WillMoveToFrontOfText() const;
	void	ShouldMoveToFrontOfText(const bool moveToFront);

	JSize		GetLineCount() const;
	JIndex		GetLineCharStart(const JIndex lineIndex) const;
	JIndex		GetLineCharEnd(const JIndex lineIndex) const;
	JIndex		GetLineForChar(const JIndex charIndex) const;
	void		GoToLine(const JIndex lineIndex);
	void		SelectLine(const JIndex lineIndex);
	JCoordinate	GetLineTop(const JIndex lineIndex) const;
	JCoordinate	GetLineBottom(const JIndex lineIndex) const;
	JSize		GetLineHeight(const JIndex lineIndex) const;
	JIndex		CRLineIndexToVisualLineIndex(const JIndex crLineIndex) const;
	JIndex		VisualLineIndexToCRLineIndex(const JIndex visualLineIndex) const;

	JCoordinate	GetCharLeft(const JIndex charIndex) const;
	JCoordinate	GetCharRight(const JIndex charIndex) const;

	JIndex	GetColumnForChar(const JIndex charIndex) const;
	void	GoToColumn(const JIndex lineIndex, const JIndex columnIndex);

	void	DeleteSelection();
	void	SelectAll();

	void	Cut();
	void	Copy() const;
	void	Paste();																// clipboard
	void	Paste(const JString& text, const JRunArray<JFont>* style = nullptr);	// other source

	bool	GetClipboard(JString* text, JRunArray<JFont>* style = nullptr) const;

	void	Paginate(const JCoordinate pageHeight,
					 JArray<JCoordinate>* breakpts) const;
	void	Print(JPagePrinter& p);

	JTEKeyHandler*	GetKeyHandler() const;
	void			SetKeyHandler(JTEKeyHandler* handler);

	bool	AllowsDragAndDrop() const;
	void	ShouldAllowDragAndDrop(const bool allow);

	static bool	WillCopyWhenSelect();
	static void	ShouldCopyWhenSelect(const bool copy);

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

	CaretMode	GetCaretMode() const;
	void		SetCaretMode(const CaretMode mode);

	JFontManager*	TEGetFontManager() const;

public:		// ought to be protected

	struct CaretLocation
	{
		JStyledText::TextIndex location;	// caret is in front of this location in the text
		JIndex lineIndex;					// caret is on this line of text

		CaretLocation()
			:
			lineIndex(0)
			{ };

		CaretLocation(const JStyledText::TextIndex& loc, const JIndex line)
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

	bool	TEIsActive() const;
	void	TEActivate();
	void	TEDeactivate();

	bool	TESelectionIsActive() const;
	void	TEActivateSelection();
	void	TEDeactivateSelection();

	bool	TECaretIsVisible() const;
	void	TEShowCaret();
	void	TEHideCaret();

	void	SetType(const Type type);

	void			RecalcAll(const bool broadcastCaretMessages = true);
	JArray<bool>	GetCmdStatus(JString* crmActionText, JString* crm2ActionText,
								 bool* isReadOnly) const;

	void			TEDraw(JPainter& p, const JRect& rect);
	virtual void	TEDrawInMargin(JPainter& p, const JPoint& topLeft,
								   const LineGeometry& geom, const JIndex lineIndex);

	bool	TEWillDragAndDrop(const JPoint& pt, const bool extendSelection,
							  const bool dropCopy) const;
	void	TEHandleMouseDown(const JPoint& pt, const JSize clickCount,
							  const bool extendSelection,
							  const bool partialWord);
	void	TEHandleMouseDrag(const JPoint& pt);
	void	TEHandleMouseUp();
	bool	TEHitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual bool	TEBeginDND() = 0;
	void			TEDNDFinished();
	void			TEHandleDNDEnter();
	void			TEHandleDNDHere(const JPoint& pt, const bool dropOnSelf);
	void			TEHandleDNDLeave();
	void			TEHandleDNDDrop(const JPoint& pt, const bool dropOnSelf,
									const bool dropCopy);
	virtual void	TEPasteDropData() = 0;

	bool	TEIsDragging() const;
	bool	TEHandleKeyPress(const JUtf8Character& key, const bool selectText,
							 const CaretMotion motion, const bool deleteToTabStop);

	JCoordinate	TEGetBoundsWidth() const;
	void		TESetBoundsWidth(const JCoordinate width);

	virtual void	TERefresh() = 0;
	virtual void	TERefreshRect(const JRect& rect) = 0;
	void			TERefreshLines(const JIndex first, const JIndex last);
	virtual void	TEUpdateDisplay();
	virtual void	TERedraw() = 0;
	virtual void	TESetGUIBounds(const JCoordinate w, const JCoordinate h,
								   const JCoordinate changeY) = 0;
	virtual bool	TEWidthIsBeyondDisplayCapacity(const JSize width) const = 0;
	virtual bool	TEScrollToRect(const JRect& rect,
								   const bool centerInDisplay) = 0;
	virtual bool	TEScrollForDrag(const JPoint& pt) = 0;
	virtual bool	TEScrollForDND(const JPoint& pt) = 0;
	virtual void	TESetVertScrollStep(const JCoordinate vStep) = 0;
	virtual void	TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const = 0;
	virtual bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const = 0;
	virtual void	TECaretShouldBlink(const bool blink) = 0;

	virtual JCoordinate	GetTabWidth(const JIndex charIndex, const JCoordinate x) const;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	bool			GetCaretLocation(CaretLocation* caretLoc) const;
	void			SetCaretLocation(const JStyledText::TextIndex& caretLoc);
	void			SetCaretLocation(const CaretLocation& caretLoc);
	CaretLocation	CalcCaretLocation(const JPoint& pt) const;
	CaretLocation	CalcCaretLocation(const JStyledText::TextIndex& index) const;
	bool			PointInSelection(const JPoint& pt) const;
	void			MoveCaretVert(const JInteger deltaLines);
	JIndex			GetColumnForChar(const CaretLocation& caretLoc) const;

	JStyledText::TextIndex	GetInsertionIndex() const;
	JStyledText::TextIndex	GetLineStart(const JIndex lineIndex) const;
	JStyledText::TextIndex	GetLineEnd(const JIndex lineIndex) const;
	JStyledText::TextCount	GetLineLength(const JIndex lineIndex) const;

	bool	GetSelection(JStyledText::TextRange* range) const;
	void	SetSelection(const JStyledText::TextRange& range,
						 const bool needCaretBcast = true,
						 const bool ignoreCopyWhenSelect = false);
	void	TEGetDoubleClickSelection(const JStyledText::TextIndex& charIndex,
									  const bool partialWord,
									  const bool dragging,
									  JStyledText::TextRange* range) const;

	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const JCoordinate tabWidth,
								  const bool breakCROnly,
								  const bool clearUndo);

	void	ReplaceSelection(const JStringMatch& match,
							 const JString& replaceStr,
							 JInterpolate* interpolator,
							 const bool preserveCase);

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

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

	Type			itsType;
	JStyledText*	itsText;
	const bool		itsOwnsTextFlag;

	bool		itsActiveFlag;
	bool		itsBreakCROnlyFlag;			// false => break line at whitespace
	bool		itsPerformDNDFlag;			// true => drag-and-drop enabled
	bool		itsMoveToFrontOfTextFlag;	// true => left arrow w/ moveEOL puts caret after whitespace
	bool		itsIsPrintingFlag;			// true => stack threads through Print()
	bool		itsDrawWhitespaceFlag;		// true => show tabs, spaces, newlines
	bool		itsAlwaysShowSelectionFlag;	// true => show selection even when not active
	CaretMode	itsCaretMode;

	JFontManager*	itsFontManager;

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

	JArray<JStyledText::TextIndex>*	itsLineStarts;	// index of first character on each line
	JRunArray<LineGeometry>*		itsLineGeom;	// geometry of each line

	JTEKeyHandler*	itsKeyHandler;

	// information for Recalc

	JStyledText::TextIndex	itsPrevTextLastIndex;	// last char index after last Recalc
	JStyledText::TextCount	itsPrevTextEnd;			// text length after last Recalc

	// used while active

	bool			itsSelActiveFlag;		// true => draw solid selection
	bool			itsCaretVisibleFlag;	// true => draw caret
	CaretLocation	itsCaret;				// insertion point is -at- this character; do not set directly - call SetCaretLocation()
	JCoordinate		itsCaretX;				// horizontal location used by MoveCaretVert()
	JFont			itsInsertionFont;		// style for characters that user types

	JStyledText::TextRange	itsSelection;	// caret is visible if this is empty; do not set directly - call SetSelection()

	// used during drag

	DragType				itsDragType;
	DragType				itsPrevDragType;	// so extend selection will maintain drag type
	JPoint					itsStartPt;
	JPoint					itsPrevPt;
	JStyledText::TextIndex	itsSelectionPivot;	// insertion point about which to pivot selection
	JStyledText::TextRange	itsWordSelPivot;	// range of characters to keep selected
	JIndex					itsLineSelPivot;	// line about which to pivot selection
	CaretLocation			itsDropLoc;			// insertion point at which to drop the dragged text
	bool					itsIsDragSourceFlag;// true => is dragging even if itsDragType == kInvalidDrag

	// global

	static bool	theCopyWhenSelectFlag;	// true => SetSelection() calls Copy()

private:

	void	Recalc(const JStyledText::TextRange& recalcRange,
				   const JStyledText::TextRange& redrawRange);
	void	RecalcRange(const JStyledText::TextRange& recalcRange,
						JCoordinate* maxLineWidth,
						JIndex* firstLineIndex, JIndex* lastLineIndex);
	void	RecalcLine(JStringIterator* textIter,
					   JRunArrayIterator<JFont>* styleIter,
					   JRunArrayIterator<LineGeometry>* geomIter,
					   const JIndex lineIndex, JCoordinate* lineWidth);
	void	GetSubwordForLine(JStringIterator* textIter,
							  JRunArrayIterator<JFont>* styleIter,
							  const JIndex lineIndex, JCoordinate* lineWidth) const;
	void	IncludeWhitespaceOnLine(JStringIterator* textIter,
									JRunArrayIterator<JFont>* styleIter,
									JCoordinate* lineWidth, bool* endOfLine) const;
	bool	NoPrevWhitespaceOnLine(const JStyledText::TextIndex& index) const;

	void	TEDrawText(JPainter& p, const JRect& rect);
	void	TEDrawLine(JPainter& p, const JCoordinate top, const LineGeometry& geom,
					   JStringIterator* textIter, JRunArrayIterator<JFont>* styleIter,
					   const JIndex lineIndex);
	void	TEDrawSelection(JPainter& p, const JRect& rect, const JIndex startVisLine,
							const JCoordinate startVisLineTop);
	void	TEDrawCaret(JPainter& p, const CaretLocation& caretLoc);

	JIndex	CalcLineIndex(const JCoordinate y, JCoordinate* lineTop) const;
	bool	TEScrollTo(const CaretLocation& caretLoc);
	JRect	CalcCaretRect(const CaretLocation& caretLoc) const;
	void	TERefreshCaret(const CaretLocation& caretLoc);

	JCoordinate	GetCharLeft(const CaretLocation& charLoc) const;
	JCoordinate	GetCharRight(const CaretLocation& charLoc) const;
	JCoordinate	GetStringWidth(const JStyledText::TextIndex& startIndex,
							   const JStyledText::TextIndex& endIndex,
							   JRunArrayIterator<JFont>* iter) const;

	JIndex	GetLineForByte(const JIndex byteIndex) const;

	void	PrivateSetBreakCROnly(const bool breakCROnly);
	void	TEGUIWidthChanged();

	JCoordinate	GetEWNHeight() const;

	void	InsertCharacter(const JUtf8Character& c);
	void	BackwardDelete(const bool deleteToTabStop,
						   JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr);
	void	ForwardDelete(const bool deleteToTabStop,
						  JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr);

	bool	LocateTab(const JStyledText::TextIndex& startIndex,
						  const JIndex endCharIndex,
						  JIndex* tabCharIndex, JIndex* pretabByteIndex) const;

	void	ReplaceRange(JStringIterator* iter, JRunArray<JFont>* styles,
						 const JStringMatch& match,
						 const JString& replaceStr,
						 const bool replaceIsRegex,
						 const bool preserveCase);

	void	BroadcastCaretMessages(const CaretLocation& caretLoc);

	static JInteger	GetLineHeight(const LineGeometry& data);

	bool			IsTrailingNewline(const JStyledText::TextIndex& index) const;
	JUtf8Character	GetCharacter(const JStyledText::TextIndex& index) const;

	// not allowed

	JTextEditor(const JTextEditor&) = delete;
	JTextEditor& operator=(const JTextEditor&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTypeChanged;
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

	class CaretLocationChanged : public JBroadcaster::Message
	{
	public:

		CaretLocationChanged(const JIndex lineIndex, const JIndex columnIndex, const JIndex charIndex)
			:
			JBroadcaster::Message(kCaretLocationChanged),
			itsLineIndex(lineIndex),
			itsColumnIndex(columnIndex),
			itsCharIndex(charIndex)
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

		JIndex
		GetCharacterIndex() const
		{
			return itsCharIndex;
		};

	private:

		JIndex itsLineIndex, itsColumnIndex, itsCharIndex;
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

inline bool
JTextEditor::IsReadOnly()
	const
{
	return itsType != kFullEditor;
}

/******************************************************************************
 TEIsActive (protected)

 ******************************************************************************/

inline bool
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
		itsActiveFlag = true;
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
		itsActiveFlag = false;
		TERefresh();
		}
}

/******************************************************************************
 TESelectionIsActive (protected)

 ******************************************************************************/

inline bool
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
		itsSelActiveFlag = true;
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
		itsSelActiveFlag = false;
		TERefresh();
		}
}

/******************************************************************************
 TECaretIsVisible (protected)

 ******************************************************************************/

inline bool
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
		itsCaretVisibleFlag = true;
		TERefreshCaret(itsCaret);
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
		itsCaretVisibleFlag = false;
		TERefreshCaret(itsCaret);
		}
}

/******************************************************************************
 GetText

 ******************************************************************************/

inline JStyledText*
JTextEditor::GetText()
{
	return itsText;
}

inline const JStyledText&
JTextEditor::GetText()
	const
{
	return *itsText;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline bool
JTextEditor::HasSelection()
	const
{
	return !itsSelection.IsEmpty();
}

inline bool
JTextEditor::GetSelection
	(
	JCharacterRange* range
	)
	const
{
	*range = itsSelection.charRange;
	return !itsSelection.IsEmpty();
}

inline bool
JTextEditor::GetSelection
	(
	JString* text
	)
	const
{
	return itsText->Copy(itsSelection, text);
}

inline bool
JTextEditor::GetSelection
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return itsText->Copy(itsSelection, text, style);
}

inline void
JTextEditor::SetSelection
	(
	const JCharacterRange&	range,
	const bool			needCaretBcast
	)
{
	SetSelection(itsText->CharToTextRange(nullptr, range), needCaretBcast);
}

// protected

inline bool
JTextEditor::GetSelection
	(
	JStyledText::TextRange* range
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
	if (!itsText->GetText().IsEmpty())
		{
		SetSelection(JStyledText::TextRange(
			JCharacterRange(1, itsText->GetText().GetCharacterCount()),
			JUtf8ByteRange( 1, itsText->GetText().GetByteCount())));
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
 CleanAllWhitespace

	Clean up the indentation whitespace and strip trailing whitespace.

 ******************************************************************************/

inline void
JTextEditor::CleanAllWhitespace
	(
	const bool align
	)
{
	SelectAll();
	CleanSelectedWhitespace(align);
}

/******************************************************************************
 Whitespace is visible

 ******************************************************************************/

inline bool
JTextEditor::WillShowWhitespace()
	const
{
	return itsDrawWhitespaceFlag;
}

inline void
JTextEditor::ShouldShowWhitespace
	(
	const bool show
	)
{
	if (show != itsDrawWhitespaceFlag)
		{
		itsDrawWhitespaceFlag = show;
		TERefresh();
		}
}

/******************************************************************************
 Showing selection

 ******************************************************************************/

inline bool
JTextEditor::WillAlwaysShowSelection()
	const
{
	return itsAlwaysShowSelectionFlag;
}

inline void
JTextEditor::ShouldAlwaysShowSelection
	(
	const bool show
	)
{
	if (show != itsAlwaysShowSelectionFlag)
		{
		itsAlwaysShowSelectionFlag = show;
		TERefresh();
		}
}

/******************************************************************************
 Move caret to front of text instead of front of line

 ******************************************************************************/

inline bool
JTextEditor::WillMoveToFrontOfText()
	const
{
	return itsMoveToFrontOfTextFlag;
}

inline void
JTextEditor::ShouldMoveToFrontOfText
	(
	const bool moveToFront
	)
{
	itsMoveToFrontOfTextFlag = moveToFront;
}

/******************************************************************************
 Automatically copy when select text (static)

	This is primarily useful under the X Window System.  It is static
	because all text should work the same way.

 ******************************************************************************/

inline bool
JTextEditor::WillCopyWhenSelect()
{
	return theCopyWhenSelectFlag;
}

inline void
JTextEditor::ShouldCopyWhenSelect
	(
	const bool copy
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

inline bool
JTextEditor::AllowsDragAndDrop()
	const
{
	return itsPerformDNDFlag;
}

inline void
JTextEditor::ShouldAllowDragAndDrop
	(
	const bool allow
	)
{
	itsPerformDNDFlag = allow;
}

/******************************************************************************
 GetCaretLocation

 ******************************************************************************/

inline bool
JTextEditor::GetCaretLocation
	(
	JIndex* charIndex
	)
	const
{
	*charIndex = itsCaret.location.charIndex;
	return itsSelection.IsEmpty();
}

inline void
JTextEditor::SetCaretLocation
	(
	const JPoint& pt
	)
{
	SetCaretLocation(CalcCaretLocation(pt));
}

// protected

inline bool
JTextEditor::GetCaretLocation
	(
	CaretLocation* caretLoc
	)
	const
{
	*caretLoc = itsCaret;
	return itsSelection.IsEmpty();
}

// protected

inline void
JTextEditor::SetCaretLocation
	(
	const JStyledText::TextIndex& index
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

inline bool
JTextEditor::WillBreakCROnly()
	const
{
	return itsBreakCROnlyFlag;
}

inline void
JTextEditor::SetBreakCROnly
	(
	const bool breakCROnly
	)
{
	if (breakCROnly != itsBreakCROnlyFlag)
		{
		PrivateSetBreakCROnly(breakCROnly);
		RecalcAll();
		}
}

/******************************************************************************
 GetInsertionCharIndex

	Return the index where new text will be typed or pasted.

 ******************************************************************************/

inline JIndex
JTextEditor::GetInsertionCharIndex()
	const
{
	return GetInsertionIndex().charIndex;
}

/******************************************************************************
 GetInsertionIndex (protected)

	Return the index where new text will be typed or pasted.

 ******************************************************************************/

inline JStyledText::TextIndex
JTextEditor::GetInsertionIndex()
	const
{
	return itsSelection.IsEmpty() ? itsCaret.location : itsSelection.GetFirst();
}

/******************************************************************************
 GetConstIteratorAtInsertionIndex

 ******************************************************************************/

inline JStringIterator*
JTextEditor::GetConstIteratorAtInsertionIndex()
	const
{
	return itsText->GetConstIterator(kJIteratorStartBefore, GetInsertionIndex());
}

/******************************************************************************
 DisposeConstIterator

 ******************************************************************************/

inline void
JTextEditor::DisposeConstIterator
	(
	JStringIterator* iter
	)
	const
{
	itsText->DisposeConstIterator(iter);
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
 GetLineCharStart

	Returns the first character on the specified line.

 ******************************************************************************/

inline JIndex
JTextEditor::GetLineCharStart
	(
	const JIndex lineIndex
	)
	const
{
	return GetLineStart(lineIndex).charIndex;
}

/******************************************************************************
 GetLineStart (protected)

	Returns the first character on the specified line.

 ******************************************************************************/

inline JStyledText::TextIndex
JTextEditor::GetLineStart
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineStarts->GetElement(lineIndex);
}

/******************************************************************************
 GetLineLength (protected)

	Returns the number of characters on the specified line.

 ******************************************************************************/

inline JStyledText::TextCount
JTextEditor::GetLineLength
	(
	const JIndex lineIndex
	)
	const
{
	const JStyledText::TextIndex
		s = GetLineStart(lineIndex),
		e = GetLineEnd(lineIndex);
	return JStyledText::TextCount(
		e.charIndex - s.charIndex + 1, e.byteIndex - s.byteIndex + 1);
}

/******************************************************************************
 GetLineCharEnd

	Returns the first character on the specified line.

 ******************************************************************************/

inline JIndex
JTextEditor::GetLineCharEnd
	(
	const JIndex lineIndex
	)
	const
{
	return GetLineEnd(lineIndex).charIndex;
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
	if (lineIndex == itsLineGeom->GetElementCount()+1 && itsText->EndsWithNewline())
		{
		return itsText->CalcInsertionFont(itsText->SelectAll().GetAfter()).GetLineHeight(itsFontManager);
		}
	else
		{
		JRunArrayIterator iter(*itsLineGeom, kJIteratorStartBefore, lineIndex);
		LineGeometry geom;
		iter.Next(&geom);
		return geom.height;
		}
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
 GetColumnForChar

	Returns the column that the specified character is in.  If the caret is
	at the far left, it is column 1.

	Given that this is only useful with monospace fonts, the CRM tab width
	is used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

inline JIndex
JTextEditor::GetColumnForChar
	(
	const JIndex charIndex
	)
	const
{
	return GetColumnForChar(CalcCaretLocation(JStyledText::TextIndex(charIndex, 0)));
}

// protected

inline JIndex
JTextEditor::GetColumnForChar
	(
	const CaretLocation& loc
	)
	const
{
	return itsText->GetColumnForChar(GetLineStart(loc.lineIndex), loc.location);
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
			GetCharLeft(CalcCaretLocation(JStyledText::TextIndex(charIndex, 0)));
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
		GetCharRight(CalcCaretLocation(JStyledText::TextIndex(charIndex, 0)));
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
		TERefreshCaret(itsCaret);
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
		JRunArrayIterator iter(itsText->GetStyles(), kJIteratorStartBefore, itsSelection.charRange.first);
		JFont f;
		iter.Next(&f);
		return f;
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontName(itsSelection, name, false);
		SetSelection(r);
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontSize(itsSelection, size, false);
		SetSelection(r);
		}
	else
		{
		itsInsertionFont.SetSize(size);
		}
}

inline void
JTextEditor::SetCurrentFontBold
	(
	const bool bold
	)
{
	if (!itsSelection.IsEmpty())
		{
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontBold(itsSelection, bold, false);
		SetSelection(r);
		}
	else
		{
		itsInsertionFont.SetBold(bold);
		}
}

inline void
JTextEditor::SetCurrentFontItalic
	(
	const bool italic
	)
{
	if (!itsSelection.IsEmpty())
		{
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontItalic(itsSelection, italic, false);
		SetSelection(r);
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontUnderline(itsSelection, count, false);
		SetSelection(r);
		}
	else
		{
		itsInsertionFont.SetUnderlineCount(count);
		}
}

inline void
JTextEditor::SetCurrentFontStrike
	(
	const bool strike
	)
{
	if (!itsSelection.IsEmpty())
		{
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontStrike(itsSelection, strike, false);
		SetSelection(r);
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontColor(itsSelection, color, false);
		SetSelection(r);
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFontStyle(itsSelection, style, false);
		SetSelection(r);
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
		const JStyledText::TextRange r = itsSelection;
		itsText->SetFont(itsSelection, f, false);
		SetSelection(r);
		}
	else
		{
		itsInsertionFont = f;
		}
}

/******************************************************************************
 Dragging (protected)

 ******************************************************************************/

inline bool
JTextEditor::TEIsDragging()
	const
{
	return itsDragType != kInvalidDrag || itsIsDragSourceFlag;
}

inline void
JTextEditor::TEDNDFinished()
{
	itsIsDragSourceFlag = false;
}

/******************************************************************************
 TEGetFontManager

 ******************************************************************************/

inline JFontManager*
JTextEditor::TEGetFontManager()
	const
{
	return itsFontManager;
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
