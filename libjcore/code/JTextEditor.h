/******************************************************************************
 JTextEditor.h

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTextEditor
#define _H_JTextEditor

#include <JBroadcaster.h>
#include <JFont.h>
#include <JRect.h>
#include <JRunArray.h>
#include <JPtrArray-JString.h>
#include <JInterpolate.h>

class JRegex;
class JStringMatch;
class JPainter;
class JPagePrinter;
class JFontManager;
class JColormap;
class JTEUndoBase;
class JTEUndoTextBase;
class JTEUndoTyping;
class JTEUndoDrop;
class JTEUndoStyle;
class JTEUndoTabShift;
class JTEKeyHandler;

typedef JBoolean (*JCharacterInWordFn)(const JUtf8Character&);

class JTextEditor : virtual public JBroadcaster
{
	friend class JTEUndoTextBase;
	friend class JTEUndoPaste;
	friend class JTEUndoDrop;
	friend class JTEUndoStyle;

	friend class JTEKeyHandler;

	friend JSize JPasteUNIXTerminalOutput(const JString& text, JTextEditor* te);

public:

	enum Type
	{
		kFullEditor,
		kSelectableText,
		kStaticText
	};

	enum PlainTextFormat
	{
		kUNIXText,
		kMacintoshText,
		kDOSText
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
		kReplaceAllBackwardCmd,
		kReplaceAllForwardCmd,
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

	struct CRMRule
	{
		JRegex*	first;		// match and replace
		JRegex* rest;		// match

		CRMRule()
			:
			first(NULL), rest(NULL)
		{ };

		CRMRule(JRegex* f, JRegex* r)
			:
			first(f), rest(r)
		{ };

		CRMRule(const JString& firstPattern, const JString& restPattern,
				const JString& replacePattern)
			:
			first(CreateFirst(firstPattern, replacePattern)),
			rest(CreateRest(restPattern))
		{ };

		static JRegex*	CreateFirst(const JString& pattern,
									const JString& replacePattern);
		static JRegex*	CreateRest(const JString& pattern);
	};

	class CRMRuleList : public JArray<CRMRule>
	{
	public:

		CRMRuleList(const JSize aBlockSize = 5)
			:
			JArray<CRMRule>(aBlockSize)
		{ };

		CRMRuleList(const CRMRuleList& source);

		void	DeleteAll();
	};

	class FontMatch
	{
	public:

		virtual ~FontMatch();

		virtual JBoolean	Match(const JFont&) const = 0;
	};

public:

	JTextEditor(const Type type, const JBoolean breakCROnly,
				const JBoolean pasteStyledText,
				const JFontManager* fontManager, JColormap* colormap,
				const JColorIndex caretColor, const JColorIndex selectionColor,
				const JColorIndex outlineColor, const JColorIndex dragColor,
				const JColorIndex wsColor, const JCoordinate width);
	JTextEditor(const JTextEditor& source);

	virtual ~JTextEditor();

	Type		GetType() const;
	JBoolean	IsReadOnly() const;
	JBoolean	WillPasteStyledText() const;

	JBoolean	WillBreakCROnly() const;
	void		SetBreakCROnly(const JBoolean breakCROnly);

	JBoolean				IsEmpty() const;
	JBoolean				EndsWithNewline() const;
	const JString&			GetText() const;
	const JRunArray<JFont>&	GetStyles() const;
	JBoolean				SetText(const JString& text,
									const JRunArray<JFont>* style = NULL);

	JBoolean	ReadPlainText(const JString& fileName, PlainTextFormat* format,
							  const JBoolean acceptBinaryFile = kJTrue);
	void		WritePlainText(const JString& fileName, const PlainTextFormat format) const;
	void		WritePlainText(std::ostream& output, const PlainTextFormat format) const;

	JBoolean	ReadPrivateFormat(std::istream& input);
	void		WritePrivateFormat(std::ostream& output) const;

	static void	WritePrivateFormat(std::ostream& output,
								   const JColormap* colormap, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange);

	JStringMatch	SearchForward(const JRegex& regex, const JBoolean entireWord,
								  const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SearchBackward(const JRegex& regex, const JBoolean entireWord,
								   const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SelectionMatches(const JRegex& regex, const JBoolean entireWord);

	JBoolean	ReplaceAllForward(const JRegex& regex,
								  const JBoolean entireWord, const JBoolean wrapSearch,
								  const JString& replaceStr, const JBoolean replaceIsRegex,
								  const JBoolean preserveCase,
								  const JBoolean restrictToSelection = kJFalse);
	JBoolean	ReplaceAllBackward(const JRegex& regex,
								   const JBoolean entireWord, const JBoolean wrapSearch,
								   const JString& replaceStr, const JBoolean replaceIsRegex,
								   const JBoolean preserveCase);

	JBoolean	SearchForward(const FontMatch& match,
							  const JBoolean wrapSearch, JBoolean* wrapped);
	JBoolean	SearchBackward(const FontMatch& match,
							   const JBoolean wrapSearch, JBoolean* wrapped);

	virtual JBoolean	TEHasSearchText() const = 0;

	const JFontManager*	TEGetFontManager() const;
	JColormap*			TEGetColormap() const;

	JFont		GetFont(const JIndex charIndex) const;

	JBoolean	SetFontName(const JIndex startIndex, const JIndex endIndex,
							const JString& name, const JBoolean clearUndo);
	JBoolean	SetFontSize(const JIndex startIndex, const JIndex endIndex,
							const JSize size, const JBoolean clearUndo);
	JBoolean	SetFontBold(const JIndex startIndex, const JIndex endIndex,
							const JBoolean bold, const JBoolean clearUndo);
	JBoolean	SetFontItalic(const JIndex startIndex, const JIndex endIndex,
							  const JBoolean italic, const JBoolean clearUndo);
	JBoolean	SetFontUnderline(const JIndex startIndex, const JIndex endIndex,
								 const JSize count, const JBoolean clearUndo);
	JBoolean	SetFontStrike(const JIndex startIndex, const JIndex endIndex,
							  const JBoolean strike, const JBoolean clearUndo);
	JBoolean	SetFontColor(const JIndex startIndex, const JIndex endIndex,
							 const JColorIndex color, const JBoolean clearUndo);
	JBoolean	SetFontStyle(const JIndex startIndex, const JIndex endIndex,
							 const JFontStyle& style, const JBoolean clearUndo);
	void		SetFont(const JIndex startIndex, const JIndex endIndex,
						const JFont& font, const JBoolean clearUndo);

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

	JBoolean	IsEntireWord() const;
	void		GoToBeginningOfLine();
	void		GoToEndOfLine();

	JFont	GetCurrentFont() const;

	void	SetCurrentFontName(const JString& name);
	void	SetCurrentFontSize(const JSize size);
	void	SetCurrentFontBold(const JBoolean bold);
	void	SetCurrentFontItalic(const JBoolean italic);
	void	SetCurrentFontUnderline(const JSize count);
	void	SetCurrentFontStrike(const JBoolean strike);
	void	SetCurrentFontColor(const JColorIndex color);
	void	SetCurrentFontStyle(const JFontStyle& style);
	void	SetCurrentFont(const JFont& font);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name);
	void	SetDefaultFontSize(const JSize size);
	void	SetDefaultFontStyle(const JFontStyle& style);
	void	SetDefaultFont(const JFont& f);

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

	JBoolean	TabInsertsSpaces() const;
	void		TabShouldInsertSpaces(const JBoolean spaces);

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

	void	Undo();
	void	Redo();
	void	ClearUndo();
	void	DeactivateCurrentUndo();

	JBoolean	IsUsingMultipleUndo() const;
	void		UseMultipleUndo(const JBoolean useMultiple = kJTrue);

	JSize		GetUndoDepth() const;
	void		SetUndoDepth(const JSize maxUndoCount);

	JBoolean	IsAtLastSaveLocation() const;
	void		SetLastSaveLocation();
	void		ClearLastSaveLocation();

	void	DeleteSelection();
	void	SelectAll();

	// clipboard

	void	Cut();
	void	Copy();
	void	Paste();

	// other source

	JBoolean	Cut(JString* text, JRunArray<JFont>* style = NULL);
	JBoolean	Copy(JString* text, JRunArray<JFont>* style = NULL) const;
	void		Paste(const JString& text, const JRunArray<JFont>* style = NULL);

	JBoolean	GetClipboard(JString* text, JRunArray<JFont>* style = NULL) const;

	static JBoolean	ContainsIllegalChars(const JString& text);
	static JBoolean	RemoveIllegalChars(JString* text, JRunArray<JFont>* style = NULL);

	void	Paginate(const JCoordinate pageHeight,
					 JArray<JCoordinate>* breakpts) const;
	void	Print(JPagePrinter& p);

	JBoolean	CleanRightMargin(const JBoolean coerce, JCharacterRange* reformatRange);

	JSize		GetCRMLineWidth() const;
	void		SetCRMLineWidth(const JSize charCount);

	JSize		GetCRMTabCharCount() const;
	void		SetCRMTabCharCount(const JSize charCount);

	JBoolean	GetCRMRuleList(const CRMRuleList** ruleList) const;
	void		SetCRMRuleList(CRMRuleList* ruleList,
							   const JBoolean teOwnsRuleList);
	void		ClearCRMRuleList();

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

	JColorIndex	GetCaretColor() const;
	void		SetCaretColor(const JColorIndex color);
	JColorIndex	GetSelectionColor() const;
	void		SetSelectionColor(const JColorIndex color);
	JColorIndex	GetSelectionOutlineColor() const;
	void		SetSelectionOutlineColor(const JColorIndex color);
	JColorIndex	GetDragColor() const;
	void		SetDragColor(const JColorIndex color);
	JColorIndex	GetWhitespaceColor() const;
	void		SetWhitespaceColor(const JColorIndex color);

	JBoolean	WillBroadcastCaretLocationChanged() const;
	void		ShouldBroadcastCaretLocationChanged(const JBoolean broadcast);

	JBoolean	WillBroadcastAllTextChanged() const;
	void		ShouldBroadcastAllTextChanged(const JBoolean broadcast);

	CaretMode	GetCaretMode() const;
	void		SetCaretMode(const CaretMode mode);

	JBoolean	CharacterIndexValid(const JIndex charIndex) const;

public:		// ought to be protected

	struct CaretLocation
	{
		JIndex charIndex;	// caret is in front of this character in the buffer
		JIndex byteIndex;	// caret is in front of this byte in the buffer
		JIndex lineIndex;	// caret is on this line of text

		CaretLocation()
			:
			charIndex(0),
			byteIndex(0),
			lineIndex(0)
		{ };

		CaretLocation(const JIndex ch, const JIndex byte, const JIndex line)
			:
			charIndex(ch),
			byteIndex(byte),
			lineIndex(line)
		{ };
	};

	struct TextIndex
	{
		JIndex charIndex;
		JIndex byteIndex;

		TextIndex()
			:
			charIndex(0),
			byteIndex(0)
		{ };

		TextIndex(const JIndex ch, const JIndex byte)
			:
			charIndex(ch),
			byteIndex(byte)
		{ };

		TextIndex(const CaretLocation& loc)
			:
			charIndex(loc.charIndex),
			byteIndex(loc.byteIndex)
		{ };
	};

	struct TextCount
	{
		JIndex charCount;
		JIndex byteCount;

		TextCount()
			:
			charCount(0),
			byteCount(0)
		{ };

		TextCount(const JIndex ch, const JIndex byte)
			:
			charCount(ch),
			byteCount(byte)
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
	void	ShouldPasteStyledText(const JBoolean pasteStyled);

	void				RecalcAll(const JBoolean needAdjustStyles);
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

	virtual void		TEDisplayBusyCursor() const = 0;

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
	virtual void		TEClipboardChanged() = 0;
	virtual JBoolean	TEGetExternalClipboard(JString* text, JRunArray<JFont>* style) const = 0;
	virtual void		TECaretShouldBlink(const JBoolean blink) = 0;

	virtual JCoordinate	GetTabWidth(const JIndex charIndex, const JCoordinate x) const;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	CRMConvertTab(JString* charBuffer, JSize* charCount,
								  const JSize currentLineWidth) const;
	virtual JSize	CRMGetTabWidth(const JIndex textColumn) const;

	virtual JBoolean	NeedsToFilterText(const JString& text) const;
	virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style);

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JCharacterRange* recalcRange, JCharacterRange* redrawRange,
											 const JBoolean deletion);

	JBoolean	IsCharacterInWord(const JUtf8Character& c) const;

	TextIndex	GetWordStart(const TextIndex& index) const;
	TextIndex	GetWordEnd(const TextIndex& index) const;
	TextIndex	GetPartialWordStart(const TextIndex& index) const;
	TextIndex	GetPartialWordEnd(const TextIndex& index) const;
	TextIndex	GetParagraphStart(const TextIndex& index) const;
	TextIndex	GetParagraphEnd(const TextIndex& index) const;

	JBoolean		GetCaretLocation(CaretLocation* caretLoc) const;
	void			SetCaretByteLocation(const JIndex byteIndex);
	CaretLocation	CalcCaretLocation(const JPoint& pt) const;
	JBoolean		PointInSelection(const JPoint& pt) const;
	void			MoveCaretVert(const JInteger deltaLines);
	JIndex			GetColumnForChar(const CaretLocation& caretLoc) const;

	TextIndex	GetInsertionIndex() const;
	TextIndex	GetLineStart(const JIndex lineIndex) const;
	TextIndex	GetLineEnd(const JIndex lineIndex) const;

	void	SetFont(const JIndex startIndex, const JRunArray<JFont>& f,
					const JBoolean clearUndo);
	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const JCoordinate tabWidth,
								  const JBoolean breakCROnly,
								  const JBoolean clearUndo);

	void	ReplaceSelection(const JStringMatch& match,
							 const JString& replaceStr,
							 const JBoolean replaceIsRegex,
							 const JBoolean preserveCase);

	static JBoolean	ReadPrivateFormat(std::istream& input, const JTextEditor* te,
									  JString* text, JRunArray<JFont>* style);

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kSelectPartialWordDrag,
		kSelectWordDrag,
		kSelectLineDrag,
		kDragAndDrop,
		kLocalDragAndDrop
	};

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

	enum CRMStatus
	{
		kFoundWord,
		kFoundNewLine,
		kFinished
	};

private:

	Type				itsType;
	JString				itsBuffer;
	JRunArray<JFont>*	itsStyles;
	JBoolean			itsActiveFlag;
	JBoolean			itsPasteStyledTextFlag;		// kJTrue => paste styled text
	JBoolean			itsPerformDNDFlag;			// kJTrue => drag-and-drop enabled
	JBoolean			itsAutoIndentFlag;			// kJTrue => auto-indent after newline enabled
	JBoolean			itsTabToSpacesFlag;			// kJTrue => 1 tab -> itsCRMTabCharCount spaces
	JBoolean			itsMoveToFrontOfTextFlag;	// kJTrue => left arrow w/ moveEOL puts caret after whitespace
	JBoolean			itsBcastLocChangedFlag;		// kJTrue => broadcast CaretLocationChanged instead of CaretLineChanged
	JBoolean			itsBcastAllTextChangedFlag;	// kJTrue => broadcast TextChanged every time
	JBoolean			itsIsPrintingFlag;			// kJTrue => stack threads through Print()
	JBoolean			itsDrawWhitespaceFlag;		// kJTrue => show tabs, spaces, newlines
	CaretMode			itsCaretMode;
	static JBoolean		theCopyWhenSelectFlag;		// kJTrue => SetSelection() calls Copy()

	const JFontManager*	itsFontMgr;
	JFont				itsDefFont;

	JColormap*			itsColormap;			// not owned
	JColorIndex			itsCaretColor;
	JColorIndex			itsSelectionColor;
	JColorIndex			itsSelectionOutlineColor;
	JColorIndex			itsDragColor;
	JColorIndex			itsWhitespaceColor;

	JTEUndoBase*			itsUndo;				// can be NULL
	JPtrArray<JTEUndoBase>*	itsUndoList;			// NULL if not multiple undo
	JIndex					itsFirstRedoIndex;		// range [1:count+1]
	JInteger				itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState				itsUndoState;
	JSize					itsMaxUndoCount;		// maximum length of itsUndoList

	JCoordinate					itsWidth;			// pixels -- width of widest line
	JCoordinate					itsHeight;			// pixels
	JCoordinate					itsGUIWidth;		// pixels -- available width of widget aperture
	JCoordinate					itsLeftMarginWidth;	// pixels
	JCoordinate					itsDefTabWidth;		// pixels
	JCoordinate					itsMaxWordWidth;	// pixels -- widest single word; only if word wrap
	JArray<TextIndex>*			itsLineStarts;		// index of first character on each line
	JRunArray<LineGeometry>*	itsLineGeom;		// geometry of each line

	JCharacterInWordFn	itsCharInWordFn;

	JInterpolate	itsInterpolator;
	JTEKeyHandler*	itsKeyHandler;

	// information for Recalc

	JBoolean	itsBreakCROnlyFlag;			// kJFalse => break line at whitespace
	JSize		itsPrevBufLength;			// buffer length after last Recalc

	// used while active

	JBoolean		itsSelActiveFlag;		// kJTrue => draw solid selection
	JBoolean		itsCaretVisibleFlag;	// kJTrue => draw caret
	CaretLocation	itsCaretLoc;			// insertion point is -at- this character; do not set directly - call SetCaretLocation()
	JCoordinate		itsCaretX;				// horizontal location used by MoveCaretVert()
	JFont			itsInsertionFont;		// style for characters that user types
	JCharacterRange	itsCharSelection;		// caret is visible if this is empty
	JUtf8ByteRange	itsByteSelection;		// matches itsCharSelection

	// used during drag

	DragType		itsDragType;
	DragType		itsPrevDragType;	// so extend selection will maintain drag type
	JPoint			itsStartPt;
	JPoint			itsPrevPt;
	JIndex			itsSelectionPivot;	// insertion point about which to pivot selection
	JCharacterRange	itsWordSelPivot;	// range of characters to keep selected
	JIndex			itsLineSelPivot;	// line about which to pivot selection
	CaretLocation	itsDropLoc;			// insertion point at which to drop the dragged text
	JBoolean		itsIsDragSourceFlag;// kJTrue => is dragging even if itsDragType == kInvalidDrag

	// information for CleanRightMargin()

	JSize	itsCRMLineWidth;
	JSize	itsCRMTabCharCount;

	CRMRuleList*	itsCRMRuleList;		// can be NULL
	JBoolean		itsOwnsCRMRulesFlag;

private:

	void		Recalc(const TextIndex& startIndex, const JSize minCharCount,
					   const JBoolean deletion,
					   const JBoolean needCaretBcast = kJTrue,
					   const JBoolean needAdjustStyles = kJTrue);
	void		Recalc(const CaretLocation& caretLoc, const JSize minCharCount,
					   const JBoolean deletion,
					   const JBoolean needCaretBcast = kJTrue,
					   const JBoolean needAdjustStyles = kJTrue);
	void		Recalc1(const JSize bufLength, const CaretLocation& caretLoc,
						const JSize minCharCount, JCoordinate* maxLineWidth,
						JIndex* firstLineIndex, JIndex* lastLineIndex);
	JSize		RecalcLine(const JSize bufLength, const JIndex firstCharIndex,
						   const JIndex lineIndex, JCoordinate* lineWidth,
						   JIndex* runIndex, JIndex* firstInRun);
	JBoolean	LocateNextWhitespace(const JSize bufLength, JIndex* startIndex) const;
	JSize		GetSubwordForLine(const JSize bufLength, const JIndex lineIndex,
								  const JIndex startIndex, JCoordinate* lineWidth) const;
	JSize		IncludeWhitespaceOnLine(const JSize bufLength, const JIndex startIndex,
										JCoordinate* lineWidth, JBoolean* endOfLine,
										JIndex* runIndex, JIndex* firstInRun) const;
	JBoolean	NoPrevWhitespaceOnLine(const JString& str, const CaretLocation& startLoc) const;

	void	TEDrawText(JPainter& p, const JRect& rect);
	void	TEDrawLine(JPainter& p, const JCoordinate top, const LineGeometry& geom,
					   const JIndex lineIndex, JIndex* runIndex, JIndex* firstInRun);
	void	TEDrawSelection(JPainter& p, const JRect& rect, const JIndex startVisLine,
							const JCoordinate startVisLineTop);
	void	TEDrawCaret(JPainter& p, const CaretLocation& caretLoc);

	static JBoolean	DefaultIsCharacterInWord(const JUtf8Character& c);

	JFont	CalcInsertionFont(const TextIndex& index) const;
	JFont	CalcInsertionFont(const JStringIterator& buffer,
							  const JRunArray<JFont>& styles) const;
	void	DropSelection(const JIndex dropLoc, const JBoolean dropCopy);

	JBoolean			GetCurrentUndo(JTEUndoBase** undo) const;
	JBoolean			GetCurrentRedo(JTEUndoBase** redo) const;
	void				NewUndo(JTEUndoBase* undo, const JBoolean isNew);
	void				SameUndo(JTEUndoBase* undo);
	void				ReplaceUndo(JTEUndoBase* oldUndo, JTEUndoBase* newUndo);
	void				ClearOutdatedUndo();
	JTEUndoTyping*		GetTypingUndo(JBoolean* isNew);
	JTEUndoStyle*		GetStyleUndo(JBoolean* isNew);
	JTEUndoTabShift*	GetTabShiftUndo(JBoolean* isNew);

	TextCount	PrivatePaste(const JString& text, const JRunArray<JFont>* style);
	TextCount	InsertText(const TextIndex& index, const JString& text,
						   const JRunArray<JFont>* style = NULL);
	TextCount	InsertText(JStringIterator* targetText, JRunArray<JFont>* targetStyle,
						   const JString& text, const JRunArray<JFont>* style,
						   const JFont* defaultStyle);
	void		DeleteText(const JCharacterRange& charRange,
						   const JUtf8ByteRange& byteRange);
	void		DeleteText(JStringIterator* iter, const JSize charCount);
	JBoolean	CleanText(const JString& text, const JRunArray<JFont>* style,
						  JString** cleanText, JRunArray<JFont>** cleanStyle,
						  JBoolean* okToInsert);

	void			SetCaretLocation(const CaretLocation& caretLoc);
	CaretLocation	CalcCaretLocation(const TextIndex& index) const;
	JIndex			CalcLineIndex(const JCoordinate y, JCoordinate* lineTop) const;
	JBoolean		TEScrollTo(const CaretLocation& caretLoc);
	JRect			CalcCaretRect(const CaretLocation& caretLoc) const;
	void			TERefreshCaret(const CaretLocation& caretLoc);

	void	SetSelection(const JCharacterRange& charRange,
						 const JUtf8ByteRange& byteRange,
						 const JBoolean needCaretBcast = kJTrue);

	JCoordinate	GetCharLeft(const CaretLocation& charLoc) const;
	JCoordinate	GetCharRight(const CaretLocation& charLoc) const;
	JCoordinate	GetCharWidth(const CaretLocation& charLoc) const;
	JCoordinate	GetStringWidth(const JIndex startIndex, const JIndex endIndex) const;
	JCoordinate	GetStringWidth(const JIndex startIndex, const JIndex endIndex,
							   JIndex* runIndex, JIndex* firstInRun) const;

	TextIndex		CharIndexToTextIndex(const JIndex charIndex) const;
	JIndex			GetLineForByte(const JIndex byteIndex) const;
	JUtf8ByteRange	CharToByteRange(const JCharacterRange& charRange) const;

	static JUtf8ByteRange	CharToByteRange(const JCharacterRange& charRange,
											JStringIterator* iter);

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

	JBoolean	PrivateCleanRightMargin(const JBoolean coerce,
										JCharacterRange* textRange,
										JString* newText, JRunArray<JFont>* newStyles,
										JIndex* newCaretIndex) const;
	JBoolean	CRMGetRange(const JIndex caretChar, const JBoolean coerce,
							JCharacterRange* range, JIndex* textStartIndex,
							JString* firstLinePrefix, JSize* firstPrefixLength,
							JString* restLinePrefix, JSize* restPrefixLength,
							JIndex* returnRuleIndex) const;
	JBoolean	CRMGetPrefix(JIndex* startChar, const JIndex endChar,
							 JString* linePrefix, JSize* prefixLength,
							 JIndex* ruleIndex) const;
	JCharacterRange	CRMMatchPrefix(const JCharacterRange& textRange, JIndex* ruleIndex) const;
	JBoolean		CRMLineMatchesRest(const JCharacterRange& range) const;
	JSize			CRMCalcPrefixLength(JString* linePrefix) const;
	JString			CRMBuildRestPrefix(const JString& firstLinePrefix,
									   const JIndex ruleIndex, JSize* prefixLength) const;
	void			CRMTossLinePrefix(JIndex* charIndex, const JIndex endChar,
									  const JIndex ruleIndex) const;
	CRMStatus		CRMReadNextWord(JIndex* charIndex, const JIndex endIndex,
									JString* spaceBuffer, JSize* spaceCount,
									JString* wordBuffer, JRunArray<JFont>* wordStyles,
									const JSize currentLineWidth,
									const JIndex origCaretIndex, JIndex* newCaretIndex,
									const JString& newText, const JBoolean requireSpace) const;
	int				CRMIsEOS(const JUtf8Character& c) const;
	void			CRMAppendWord(JString* newText, JRunArray<JFont>* newStyles,
								  JSize* currentLineWidth, JIndex* newCaretIndex,
								  const JString& spaceBuffer, const JSize spaceCount,
								  const JString& wordBuffer, const JRunArray<JFont>& wordStyles,
								  const JString& linePrefix, const JSize prefixLength) const;

	JBoolean	LocateTab(const JIndex startIndex, const JIndex endIndex,
						  JIndex* tabIndex) const;

	JStringMatch	SearchForward(const JString& buffer, const TextIndex& startIndex,
								  const JRegex& regex, const JBoolean entireWord,
								  const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SearchBackward(const JString& buffer, const TextIndex& startIndex,
								   const JRegex& regex, const JBoolean entireWord,
								   const JBoolean wrapSearch, JBoolean* wrapped);

	void		ReplaceRange(JStringIterator* buffer, JRunArray<JFont>* styles,
							 const JStringMatch& match,
							 const JString& replaceStr,
							 const JBoolean replaceIsRegex,
							 const JBoolean preserveCase);
	JBoolean	IsEntireWord(const JString& buffer,
							 const JCharacterRange& charRange,
							 const JUtf8ByteRange& byteRange) const;

	static void	WritePrivateFormat(std::ostream& output,
								   const JColormap* colormap, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange,
								   const JUtf8ByteRange& byteRange);

	void		BroadcastCaretMessages(const CaretLocation& caretLoc,
									   const JBoolean lineChanged);
	JBoolean	BroadcastCaretPosChanged(const CaretLocation& caretLoc);

	static JInteger	GetLineHeight(const LineGeometry& data);

	static void	ConvertFromMacintoshNewlinetoUNIXNewline(JString* buffer);

	static JListT::CompareResult
		CompareCharacterIndices(const TextIndex& i, const TextIndex& j);
	static JListT::CompareResult
		CompareByteIndices(const TextIndex& i, const TextIndex& j);

	// not allowed

	const JTextEditor& operator=(const JTextEditor& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTypeChanged;
	static const JUtf8Byte* kTextSet;
	static const JUtf8Byte* kTextChanged;
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

	class TextSet : public JBroadcaster::Message
		{
		public:

			TextSet()
				:
				JBroadcaster::Message(kTextSet)
				{ };
		};

	class TextChanged : public JBroadcaster::Message
		{
		public:

			TextChanged()
				:
				JBroadcaster::Message(kTextChanged)
				{ };
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
 IsEmpty

 ******************************************************************************/

inline JBoolean
JTextEditor::IsEmpty()
	const
{
	return itsBuffer.IsEmpty();
}

/******************************************************************************
 GetText

 ******************************************************************************/

inline const JString&
JTextEditor::GetText()
	const
{
	return itsBuffer;
}

/******************************************************************************
 CharacterIndexValid

 ******************************************************************************/

inline JBoolean
JTextEditor::CharacterIndexValid
	(
	const JIndex charIndex
	)
	const
{
	return itsBuffer.CharacterIndexValid(charIndex);
}

/******************************************************************************
 GetStyles

 ******************************************************************************/

inline const JRunArray<JFont>&
JTextEditor::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline JBoolean
JTextEditor::HasSelection()
	const
{
	return !itsCharSelection.IsEmpty();
}

inline JBoolean
JTextEditor::GetSelection
	(
	JCharacterRange* range
	)
	const
{
	*range = itsCharSelection;
	return !itsCharSelection.IsEmpty();
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
/******************************************************************************
 Multiple undo

 ******************************************************************************/

inline JBoolean
JTextEditor::IsUsingMultipleUndo()
	const
{
	return JI2B( itsUndoList != NULL );
}

inline JSize
JTextEditor::GetUndoDepth()
	const
{
	return itsMaxUndoCount;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline JBoolean
JTextEditor::IsAtLastSaveLocation()
	const
{
	return JI2B( itsUndoList != NULL &&
				 itsLastSaveRedoIndex > 0 &&
				 JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex );
}

inline void
JTextEditor::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
JTextEditor::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the text is a single, complete word.

 ******************************************************************************/

inline JBoolean
JTextEditor::IsEntireWord()
	const
{
	return IsEntireWord(itsBuffer,
						JCharacterRange(1, itsBuffer.GetCharacterCount()),
						JUtf8ByteRange(1, itsBuffer.GetByteCount()));
}

/******************************************************************************
 TEGetFontManager

 ******************************************************************************/

inline const JFontManager*
JTextEditor::TEGetFontManager()
	const
{
	return itsFontMgr;
}

/******************************************************************************
 TEGetColormap

 ******************************************************************************/

inline JColormap*
JTextEditor::TEGetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 Allow paste styled text

	ShouldPasteStyled() is protected because most derived classes won't
	be written to expect it to change.

 ******************************************************************************/

inline JBoolean
JTextEditor::WillPasteStyledText()
	const
{
	return itsPasteStyledTextFlag;
}

// protected

inline void
JTextEditor::ShouldPasteStyledText
	(
	const JBoolean pasteStyled
	)
{
	itsPasteStyledTextFlag = pasteStyled;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline const JFont&
JTextEditor::GetDefaultFont()
	const
{
	return itsDefFont;
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
		RecalcAll(kJFalse);
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
 Tab inserts spaces

 ******************************************************************************/

inline JBoolean
JTextEditor::TabInsertsSpaces()
	const
{
	return itsTabToSpacesFlag;
}

inline void
JTextEditor::TabShouldInsertSpaces
	(
	const JBoolean spaces
	)
{
	itsTabToSpacesFlag = spaces;
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
	CleanWhitespace(JCharacterRange(1, itsBuffer.GetCharacterCount()), align);
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
 CleanRightMargin() parameters

 ******************************************************************************/

inline JSize
JTextEditor::GetCRMLineWidth()
	const
{
	return itsCRMLineWidth;
}

inline JSize
JTextEditor::GetCRMTabCharCount()
	const
{
	return itsCRMTabCharCount;
}

inline JBoolean
JTextEditor::GetCRMRuleList
	(
	const CRMRuleList** ruleList
	)
	const
{
	*ruleList = itsCRMRuleList;
	return JI2B( itsCRMRuleList != NULL );
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
	*charIndex = itsCaretLoc.charIndex;
	return itsCharSelection.IsEmpty();
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
	return itsCharSelection.IsEmpty();
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

inline JTextEditor::TextIndex
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
	return itsLeftMarginWidth + GetCharLeft(CalcCaretLocation(TextIndex(charIndex, 0)));
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
	return itsLeftMarginWidth + GetCharRight(CalcCaretLocation(TextIndex(charIndex, 0)));
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
 Always broadcast TextChanged

	If this is set, TextChanged is always broadcast instead of optimizing
	out redundant messages generated by the active undo object.

 ******************************************************************************/

inline JBoolean
JTextEditor::WillBroadcastAllTextChanged()
	const
{
	return itsBcastAllTextChangedFlag;
}

inline void
JTextEditor::ShouldBroadcastAllTextChanged
	(
	const JBoolean broadcast
	)
{
	itsBcastAllTextChangedFlag = broadcast;
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
 Font access (protected)

 ******************************************************************************/

inline JFont
JTextEditor::GetFont
	(
	const JIndex charIndex
	)
	const
{
	return itsStyles->GetElement(charIndex);
}

/******************************************************************************
 EndsWithNewline

 ******************************************************************************/

inline JBoolean
JTextEditor::EndsWithNewline()
	const
{
	return JConvertToBoolean(
				!itsBuffer.IsEmpty() &&
				itsBuffer.GetLastCharacter() == '\n' );
}

/******************************************************************************
 Colors

 ******************************************************************************/

inline JColorIndex
JTextEditor::GetCaretColor()
	const
{
	return itsCaretColor;
}

inline void
JTextEditor::SetCaretColor
	(
	const JColorIndex color
	)
{
	if (color != itsCaretColor)
		{
		itsCaretColor = color;
		TERefreshCaret(itsCaretLoc);
		}
}

inline JColorIndex
JTextEditor::GetSelectionColor()
	const
{
	return itsSelectionColor;
}

inline void
JTextEditor::SetSelectionColor
	(
	const JColorIndex color
	)
{
	if (color != itsSelectionColor)
		{
		itsSelectionColor = color;
		if (!itsCharSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorIndex
JTextEditor::GetSelectionOutlineColor()
	const
{
	return itsSelectionOutlineColor;
}

inline void
JTextEditor::SetSelectionOutlineColor
	(
	const JColorIndex color
	)
{
	if (color != itsSelectionOutlineColor)
		{
		itsSelectionOutlineColor = color;
		if (!itsCharSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorIndex
JTextEditor::GetDragColor()
	const
{
	return itsDragColor;
}

inline void
JTextEditor::SetDragColor
	(
	const JColorIndex color
	)
{
	itsDragColor = color;
}

inline JColorIndex
JTextEditor::GetWhitespaceColor()
	const
{
	return itsWhitespaceColor;
}

inline void
JTextEditor::SetWhitespaceColor
	(
	const JColorIndex color
	)
{
	if (color != itsWhitespaceColor)
		{
		itsWhitespaceColor = color;
		TERefresh();
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
	return (l1.charIndex == l2.charIndex);
}

inline int
operator!=
	(
	const JTextEditor::CaretLocation& l1,
	const JTextEditor::CaretLocation& l2
	)
{
	return (l1.charIndex != l2.charIndex);
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
