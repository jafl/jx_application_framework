/******************************************************************************
 JTextEditor.cpp

	Class to edit styled text.

	CaretLineChanged is broadcast by default.  If anybody requests
	CaretLocationChanged, CaretLineChanged will no longer be broadcast,
	since it duplicates the information.  Generic classes should accept
	either message to avoid depending on the setting.

	Only public and protected functions are required to call NewUndo(),
	and only if the action to be performed changes the text or styles.
	Private functions must -not- call NewUndo(), because several manipulations
	may be required to perform one user command, and only the user command
	as a whole is undoable.  (Otherwise, the user may get confused.)

	Because of this convention, public functions that affect undo should only
	be a public interface to a private function.  The public function calls
	NewUndo() and then calls the private function.  The private function
	does all the work, but doesn't modify the undo information.  This allows other
	private functions to use the routine (private version) without modifying the
	undo information.

	In order to guarantee that the TextChanged message means "text has already
	changed", NewUndo() must be called -after- making the modification.
	(even though the Undo object has to be created before the modifications)

	TextSet is different from TextChanged because documents will typically
	use only the latter for setting their "unsaved" flag.

	When the text ends with a newline, we have to draw the caret on the
	next line.  This is a special case because (charIndex == bufLength+1)
	would normally catch this.  We handle this special case internally
	rather than appending an extra item to itsLineStarts because this keeps
	the issue internal rather than forcing clients to deal with it.

	Derived classes must implement the following routines:

		TERefresh
			Put an update event in the queue to redraw the text.

		TERefreshRect
			Put an update event in the queue to redraw part of the text.

		TEUpdateDisplay
			Redraw whatever was requested by TERefresh() and TERefreshRect().
			(This is not pure virtual because some systems automatically
			 update the window after each event.)

		TERedraw
			Redraw the text immediately.

		TESetGUIBounds
			Set the bounds of the GUI object to match our size.  The original
			height and vertical position of the change can be used to update
			scrolltabs.  If change position is negative, don't update the
			scrolltabs.

		TEWidthIsBeyondDisplayCapacity
			Returns kJTrue if the given bounds width is too wide for the
			current graphics system to handle.  If so, breakCROnly is
			turned off.

		TEScrollToRect
			Scroll the text to make the given rectangle visible, possibly
			centering it in the middle of the display.  Return kJTrue
			if scrolling was necessary.

		TEScrollForDrag
			Scroll the text to make the given point visible.  Return kJTrue
			if scrolling was necessary.

		TEScrollForDND
			Same as TEScrollForDrag(), but separate, since some frameworks
			do the scrolling automatically.

		TESetVertScrollStep
			Set the vertical step size and page context used when scrolling
			the text.

		TECaretShouldBlink
			If blink is kJTrue, reset the timer and make the caret blink by
			calling TEShow/HideCaret().  Otherwise, call TEHideCaret().
			The derived class' constructor must call TECaretShouldBlink(kJTrue)
			because it is pure virtual for us.

		TEClipboardChanged
			Do whatever is appropriate to update the system clipboard
			after a Copy or Cut operation.

		TEGetExternalClipboard
			Returns kJTrue if there is something pasteable on the system
			clipboard.

		TEBeginDND
			Returns kJTrue if it is able to start a Drag-And-Drop session
			with the OS.  From then on, the derived class should call the
			TEHandleDND*() functions.  It must also call TEDNDFinished()
			when the drag ends.  If TEBeginDND() returns kJFalse, then we
			will manage an internal DND session.  In this case, the derived
			class should continue to call the usual TEHandleMouse*()
			functions.

		TEPasteDropData
			Get the data that was dropped and use Paste(text,style)
			to insert it.  (Before this is called, the insertion point
			is set so that Paste() will work correctly.)

		TEHasSearchText
			Return kJTrue if the user has entered any text for which to search.

	To draw page headers and footers while printing, override the
	following routines:

		GetPrintHeaderHeight
			Return the height required for the page header.

		DrawPrintHeader
			Draw the page header.  JTable will lock the header afterwards.

		GetPrintFooterHeight
			Return the height required for the page footer.

		DrawPrintFooter
			Draw the page footer.  JTable will lock the footer afterwards.

	The default implementation of tabs rounds the location up to the nearest
	multiple of itsDefTabWidth.  To implement non-uniform tabs or tabs on a
	line-by-line basis, override the following function:

		GetTabWidth
			Given the index of the tab character (charIndex) and the horizontal
			position on the line (in pixels) where the tab character starts (x),
			return the width of the tab character.

	Functionality yet to be implemented:

		Search & replace for text and styles combined

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JTextEditor.h>
#include <JTEUndoTyping.h>
#include <JTEUndoPaste.h>
#include <JTEUndoDrop.h>
#include <JTEUndoStyle.h>
#include <JTEUndoTabShift.h>
#include <JTEKeyHandler.h>
#include <JPagePrinter.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JListUtil.h>
#include <JRunArrayIterator.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JLatentPG.h>
#include <JMinMax.h>
#include <jTextUtil.h>
#include <jASCIIConstants.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jMouseUtil.h>
#include <jFileUtil.h>
#include <jTime.h>
#include <ctype.h>
#include <jGlobals.h>
#include <sstream>
#include <jAssert.h>

const JCoordinate kDefLeftMarginWidth = 10;
const JCoordinate kRightMarginWidth   = 2;

const JCoordinate kDraggedOutlineRadius = 10;

const JFileVersion kCurrentPrivateFormatVersion = 1;

const JSize kDefaultMaxUndoCount = 100;

const JSize kUNIXLineWidth    = 75;
const JSize kUNIXTabCharCount = 8;

JBoolean JTextEditor::theCopyWhenSelectFlag = kJFalse;

// JBroadcaster message types

const JUtf8Byte* JTextEditor::kTypeChanged          = "TypeChanged::JTextEditor";
const JUtf8Byte* JTextEditor::kTextSet              = "TextSet::JTextEditor";
const JUtf8Byte* JTextEditor::kTextChanged          = "TextChanged::JTextEditor";
const JUtf8Byte* JTextEditor::kCaretLineChanged     = "CaretLineChanged::JTextEditor";
const JUtf8Byte* JTextEditor::kCaretLocationChanged = "CaretLocationChanged::JTextEditor";

/******************************************************************************
 Constructor

	We don't provide a constructor that accepts text because we
	can't call RecalcAll() due to pure virtual functions.

	*** Derived classes must call RecalcAll().

 ******************************************************************************/

JTextEditor::JTextEditor
	(
	const Type			type,
	const JBoolean		breakCROnly,
	const JBoolean		pasteStyledText,
	const JFontManager*	fontManager,
	JColormap*			colormap,
	const JColorIndex	caretColor,
	const JColorIndex	selectionColor,
	const JColorIndex	outlineColor,
	const JColorIndex	dragColor,
	const JColorIndex	wsColor,
	const JCoordinate	width
	)
	:
	itsType(type),
	itsPasteStyledTextFlag(pasteStyledText),

	itsFontMgr(fontManager),
	itsDefFont(fontManager->GetDefaultFont()),
	itsColormap(colormap),

	itsCaretColor(caretColor),
	itsSelectionColor(selectionColor),
	itsSelectionOutlineColor(outlineColor),
	itsDragColor(dragColor),
	itsWhitespaceColor(wsColor),

	itsKeyHandler(NULL),

	itsInsertionFont(itsDefFont)
{
	itsStyles = jnew JRunArray<JFont>;
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = kDefaultMaxUndoCount;
	itsActiveFlag              = kJFalse;
	itsSelActiveFlag           = kJFalse;
	itsCaretVisibleFlag        = kJFalse;
	itsPerformDNDFlag          = kJFalse;
	itsAutoIndentFlag          = kJFalse;
	itsTabToSpacesFlag         = kJFalse;
	itsMoveToFrontOfTextFlag   = kJFalse;
	itsBcastLocChangedFlag     = kJFalse;
	itsBcastAllTextChangedFlag = kJFalse;
	itsBreakCROnlyFlag         = breakCROnly;
	itsIsPrintingFlag          = kJFalse;
	itsDrawWhitespaceFlag      = kJFalse;
	itsCaretMode               = kLineCaret;

	itsWidth           = width - kDefLeftMarginWidth - kRightMarginWidth;
	itsHeight          = 0;
	itsGUIWidth        = itsWidth;
	itsLeftMarginWidth = kDefLeftMarginWidth;
	itsDefTabWidth     = 36;	// 1/2 inch
	itsMaxWordWidth    = 0;

	itsLineStarts = jnew JArray<TextIndex>;
	assert( itsLineStarts != NULL );
	itsLineStarts->SetCompareFunction(CompareCharacterIndices);
	itsLineStarts->SetSortOrder(JListT::kSortAscending);

	itsLineGeom = jnew JRunArray<LineGeometry>;
	assert( itsLineGeom != NULL );

	itsCaretLoc         = CaretLocation(1,1,1);
	itsCaretX           = 0;
	itsInsertionFont    = CalcInsertionFont(TextIndex(1,1));

	itsPrevDragType     = itsDragType = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;

	itsPrevBufLength    = 0;
	itsCRMLineWidth     = kUNIXLineWidth;
	itsCRMTabCharCount  = kUNIXTabCharCount;
	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;

	itsCharInWordFn     = DefaultIsCharacterInWord;

	if (type == kFullEditor)
		{
		itsBuffer.SetBlockSize(4096);
		itsStyles->SetBlockSize(128);
		itsLineStarts->SetBlockSize(128);
		itsLineGeom->SetBlockSize(128);
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTextEditor::JTextEditor
	(
	const JTextEditor& source
	)
	:
	JBroadcaster(source),

	itsType( source.itsType ),
	itsBuffer( source.itsBuffer ),
	itsPasteStyledTextFlag( source.itsPasteStyledTextFlag ),

	itsFontMgr( source.itsFontMgr ),
	itsDefFont( source.itsDefFont ),
	itsColormap( source.itsColormap ),

	itsCaretColor( source.itsCaretColor ),
	itsSelectionColor( source.itsSelectionColor ),
	itsSelectionOutlineColor( source.itsSelectionOutlineColor ),
	itsDragColor( source.itsDragColor ),

	itsInsertionFont( source.itsInsertionFont )
{
	itsStyles = jnew JRunArray<JFont>(*(source.itsStyles));
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = source.itsMaxUndoCount;
	itsActiveFlag              = kJFalse;
	itsSelActiveFlag           = kJFalse;
	itsCaretVisibleFlag        = kJFalse;
	itsPerformDNDFlag          = source.itsPerformDNDFlag;
	itsAutoIndentFlag          = source.itsAutoIndentFlag;
	itsTabToSpacesFlag         = source.itsTabToSpacesFlag;
	itsMoveToFrontOfTextFlag   = source.itsMoveToFrontOfTextFlag;
	itsBcastLocChangedFlag     = source.itsBcastLocChangedFlag;
	itsBcastAllTextChangedFlag = source.itsBcastAllTextChangedFlag;
	itsBreakCROnlyFlag         = source.itsBreakCROnlyFlag;
	itsIsPrintingFlag          = kJFalse;

	itsWidth           = source.itsWidth;
	itsHeight          = source.itsHeight;
	itsGUIWidth        = source.itsGUIWidth;
	itsLeftMarginWidth = source.itsLeftMarginWidth;
	itsDefTabWidth     = source.itsDefTabWidth;
	itsMaxWordWidth    = source.itsMaxWordWidth;

	itsLineStarts = jnew JArray<TextIndex>(*(source.itsLineStarts));
	assert( itsLineStarts != NULL );

	itsLineGeom = jnew JRunArray<LineGeometry>(*(source.itsLineGeom));
	assert( itsLineGeom != NULL );

	itsPrevBufLength = source.itsPrevBufLength;

	itsCaretLoc         = CaretLocation(1,1,1);
	itsCaretX           = 0;
	itsInsertionFont    = CalcInsertionFont(TextIndex(1,1));

	itsPrevDragType     = itsDragType = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;

	itsCRMLineWidth     = source.itsCRMLineWidth;
	itsCRMTabCharCount  = source.itsCRMTabCharCount;

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
	if (source.itsCRMRuleList != NULL)
		{
		SetCRMRuleList(source.itsCRMRuleList, source.itsOwnsCRMRulesFlag);
		}

	itsCharInWordFn   = source.itsCharInWordFn;
	itsKeyHandler     = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextEditor::~JTextEditor()
{
	jdelete itsStyles;
	jdelete itsUndo;
	jdelete itsLineStarts;
	jdelete itsLineGeom;
	jdelete itsUndoList;
	jdelete itsKeyHandler;

	ClearCRMRuleList();
}

/******************************************************************************
 SetType (protected)

	This is protected because most derived classes will not be written
	to expect the type to change.

	Broadcasts TypeChanged.

 ******************************************************************************/

void
JTextEditor::SetType
	(
	const Type type
	)
{
	itsType = type;
	TERefresh();
	Broadcast(TypeChanged(type));
}

/******************************************************************************
 SetText

	Returns kJFalse if illegal characters had to be removed.

	This should not be accessible to the user, so we don't provide Undo.

	style can safely be NULL or itsStyles.

 ******************************************************************************/

JBoolean
JTextEditor::SetText
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	if (TEIsDragging())
		{
		return kJTrue;
		}

	ClearUndo();
	itsBuffer = text;

	JBoolean cleaned = kJFalse;
	if (style != NULL)
		{
		assert( itsBuffer.GetCharacterCount() == style->GetElementCount() );
		*itsStyles = *style;
		cleaned    = RemoveIllegalChars(&itsBuffer, itsStyles);
		}
	else
		{
		cleaned = RemoveIllegalChars(&itsBuffer);

		itsStyles->RemoveAll();
		if (!itsBuffer.IsEmpty())
			{
			itsStyles->AppendElements(itsDefFont, itsBuffer.GetCharacterCount());
			}
		else
			{
			itsInsertionFont = itsDefFont;
			}
		}

	if (NeedsToFilterText(itsBuffer))
		{
		cleaned = kJTrue;
		if (!FilterText(&itsBuffer, itsStyles))
			{
			itsBuffer.Clear();
			itsStyles->RemoveAll();
			}
		}

	RecalcAll(kJTrue);
	SetCaretLocation(1);
	Broadcast(TextSet());
	return !cleaned;
}

/******************************************************************************
 ReadPlainText

	If acceptBinaryFile == kJTrue, returns kJFalse if illegal characters had to
	be removed.  If acceptBinaryFile == kJFalse, returns kJFalse without loading
	the file if the file contains illegal characters.

	We read directly into itsBuffer to avoid making two copies of the
	file's data.  (The file could be very large.)

 ******************************************************************************/

static const JUtf8Byte  kUNIXNewlineChar      = '\n';
static const JUtf8Byte* kMacintoshNewline     = "\r";
static const JUtf8Byte  kMacintoshNewlineChar = '\r';
static const JUtf8Byte* kDOSNewline           = "\r\n";
static const JUtf8Byte  k1stDOSNewlineChar    = '\r';

JBoolean
JTextEditor::ReadPlainText
	(
	const JString&		fileName,
	PlainTextFormat*	format,
	const JBoolean		acceptBinaryFile
	)
{
	JReadFile(fileName, &itsBuffer);

	JIndex i;
	if (ContainsIllegalChars(itsBuffer))
		{
		if (!acceptBinaryFile)
			{
			return kJFalse;
			}

		// It is probably a binary file, so we shouldn't mess with it.

		*format = kUNIXText;
		}

	else if (itsBuffer.Contains(kDOSNewline))
		{
		*format = kDOSText;

		TEDisplayBusyCursor();

		JSize byteCount;
		JGetFileLength(fileName, &byteCount);

		JLatentPG pg(100);
		pg.FixedLengthProcessBeginning(byteCount,
				JGetString("ConvertFromDOS::JTextEditor"), kJFalse, kJFalse);

		// Converting itsBuffer in memory is more than 100 times slower,
		// but we have to normalize the bytes after reading, so we have to
		// allocate a new block.

		JUtf8Byte* buffer = jnew JUtf8Byte[ byteCount ];
		assert( buffer != NULL );

		std::ifstream input(fileName.GetBytes());
		JIndex i = 0;
		while (1)
			{
			const JUtf8Byte c = input.get();
			if (input.eof() || input.fail())
				{
				break;
				}

			if (c != k1stDOSNewlineChar)
				{
				buffer[i] = c;
				i++;
				}
			else
				{
				pg.IncrementProgress(i - pg.GetCurrentStepCount());
				}
			}
		input.close();

		itsBuffer.Set(buffer, byteCount);
		jdelete [] buffer;

		pg.ProcessFinished();
		}

	else if (itsBuffer.Contains(kMacintoshNewline))
		{
		*format = kMacintoshText;

		ConvertFromMacintoshNewlinetoUNIXNewline(&itsBuffer);
		}

	else
		{
		*format = kUNIXText;
		}

	return SetText(itsBuffer, NULL);
}

/******************************************************************************
 ConvertFromMacintoshNewlinetoUNIXNewline (private static)

	Efficient hack to replace bytes in place.

 ******************************************************************************/

void
JTextEditor::ConvertFromMacintoshNewlinetoUNIXNewline
	(
	JString* buffer
	)
{
	JUtf8Byte* s = const_cast<JUtf8Byte*>(buffer->GetBytes());
	while (*s > 0)
		{
		if (*s == kMacintoshNewlineChar)
			{
			*s = kUNIXNewlineChar;
			}
		*s++;
		}
}

/******************************************************************************
 WritePlainText

 ******************************************************************************/

void
JTextEditor::WritePlainText
	(
	const JString&			fileName,
	const PlainTextFormat	format
	)
	const
{
	std::ofstream output(fileName.GetBytes());
	WritePlainText(output, format);
}

void
JTextEditor::WritePlainText
	(
	std::ostream&			output,
	const PlainTextFormat	format
	)
	const
{
	if (format == kUNIXText)
		{
		itsBuffer.Print(output);
		return;
		}

	const JUtf8Byte* newlineStr = NULL;
	if (format == kDOSText)
		{
		newlineStr = kDOSNewline;
		}
	else if (format == kMacintoshText)
		{
		newlineStr = kMacintoshNewline;
		}
	assert( newlineStr != NULL );

	const JUtf8Byte* buffer = itsBuffer.GetBytes();
	const JSize byteCount   = itsBuffer.GetByteCount();
	JIndex start            = 0;
	for (JIndex i=0; i<byteCount; i++)
		{
		if (buffer[i] == kUNIXNewlineChar)
			{
			if (start < i)
				{
				output.write(buffer + start, i - start);
				}
			output << newlineStr;
			start = i+1;
			}
		}

	if (start < byteCount)
		{
		output.write(buffer + start, byteCount - start);
		}
}

/******************************************************************************
 ReadPrivateFormat

	See WritePrivateFormat() for version information.

 ******************************************************************************/

JBoolean
JTextEditor::ReadPrivateFormat
	(
	std::istream& input
	)
{
	TEDisplayBusyCursor();

	JString text;
	JRunArray<JFont> style;
	if (ReadPrivateFormat(input, this, &text, &style))
		{
		SetText(text, &style);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadPrivateFormat (static protected)

	See WritePrivateFormat() for version information.

 ******************************************************************************/

JBoolean
JTextEditor::ReadPrivateFormat
	(
	std::istream&		input,
	const JTextEditor*	te,
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	text->Clear();
	style->RemoveAll();

	// version

	JFileVersion vers;
	input >> vers;

	if (vers > kCurrentPrivateFormatVersion)
		{
		return kJFalse;
		}

	// text

	JSize textLength;
	input >> textLength;
	input.ignore(1);
	if (textLength > 0)
		{
		text->Read(input, textLength);
		}

	// list of font names

	JSize fontCount;
	input >> fontCount;

	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=fontCount; i++)
		{
		JString* name = jnew JString;
		assert( name != NULL );
		input >> *name;
		fontList.Append(name);
		}

	// list of rgb values

	JSize colorCount;
	input >> colorCount;

	JArray<JColorIndex> colorList;

	JRGB color;
	for (JIndex i=1; i<=colorCount; i++)
		{
		input >> color;
		colorList.AppendElement(te->itsColormap->GetColor(color));
		}

	// styles

	JSize runCount;
	input >> runCount;

	const JFontManager* fontMgr = te->TEGetFontManager();

	JSize size;
	JFontStyle fStyle;
	for (JIndex i=1; i<=runCount; i++)
		{
		JSize charCount;
		input >> charCount;

		JIndex fontIndex;
		input >> fontIndex;

		input >> size;
		input >> fStyle.bold >> fStyle.italic >> fStyle.strike;
		input >> fStyle.underlineCount;

		JIndex colorIndex;
		input >> colorIndex;
		fStyle.color = colorList.GetElement(colorIndex);

		style->AppendElements(
			fontMgr->GetFont(*(fontList.GetElement(fontIndex)), size, fStyle),
			charCount);
		}

	return kJTrue;
}

/******************************************************************************
 WritePrivateFormat

 ******************************************************************************/

void
JTextEditor::WritePrivateFormat
	(
	std::ostream& output
	)
	const
{
	if (!IsEmpty())
		{
		WritePrivateFormat(output, itsColormap, kCurrentPrivateFormatVersion,
						   itsBuffer, *itsStyles,
						   JCharacterRange(1, itsBuffer.GetCharacterCount()),
						   JUtf8ByteRange(1, itsBuffer.GetByteCount()));
		}
	else
		{
		output << kCurrentPrivateFormatVersion;
		output << " 0 0 0 0";
		}
}

/******************************************************************************
 WritePrivateFormat (static)

	We have to be able to write each version because this is what we
	put on the clipboard.

	version 1:  initial version.

 ******************************************************************************/

void
JTextEditor::WritePrivateFormat
	(
	std::ostream&			output,
	const JColormap*		colormap,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const JCharacterRange&	charRange
	)
{
	assert( !charRange.IsEmpty() );
	assert( text.RangeValid(charRange) );

	JUtf8ByteRange byteRange;
	if (charRange.first == 1 && charRange.last == text.GetCharacterCount())
		{
		byteRange.Set(1, text.GetCharacterCount());
		}
	else
		{
		JStringIterator iter(text);
		byteRange = CharToByteRange(charRange, &iter);
		}

	WritePrivateFormat(output, colormap, vers, text, style, charRange, byteRange);
}

// static private

void
JTextEditor::WritePrivateFormat
	(
	std::ostream&			output,
	const JColormap*		colormap,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const JCharacterRange&	charRange,
	const JUtf8ByteRange&	byteRange
	)
{
	assert( vers <= kCurrentPrivateFormatVersion );

	// write version

	output << vers;

	// write text efficiently

	if (charRange.first == 1 && charRange.last == text.GetCharacterCount())
		{
		output << ' ' << text.GetCharacterCount() << ' ';
		output.write(text.GetRawBytes(), text.GetByteCount());
		}
	else
		{
		output << ' ' << JString::CountCharacters(text.GetRawBytes(), byteRange) << ' ';
		output.write(text.GetRawBytes() + byteRange.first - 1, byteRange.GetCount());
		}

	// build lists of font names and colors

	JSize styleRunCount = 0;
	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	fontList.SetCompareFunction(JCompareStringsCaseSensitive);
	JArray<JColorIndex> colorList;
	colorList.SetCompareFunction(JCompareIndices);

	JIndex startRunIndex, startFirstInRun;
	JBoolean found = style.FindRun(charRange.first, &startRunIndex, &startFirstInRun);
	assert( found );

	JIndex i          = charRange.first;
	JIndex runIndex   = startRunIndex;
	JIndex firstInRun = startFirstInRun;
	do
		{
		const JFont& f   = style.GetRunDataRef(runIndex);
		JString fontName = f.GetName();
		const JIndex fontIndex =
			fontList.SearchSorted1(&fontName, JListT::kAnyMatch, &found);
		if (!found)
			{
			fontList.InsertAtIndex(fontIndex, fontName);
			}

		const JColorIndex color = f.GetStyle().color;
		const JIndex colorIndex =
			colorList.SearchSorted1(color, JListT::kAnyMatch, &found);
		if (!found)
			{
			colorList.InsertElementAtIndex(colorIndex, color);
			}

		i += style.GetRunLength(runIndex) - (i - firstInRun);
		runIndex++;
		firstInRun = i;
		styleRunCount++;
		}
		while (i <= charRange.last);

	// write list of font names

	const JSize fontCount = fontList.GetElementCount();
	output << ' ' << fontCount;

	for (i=1; i<=fontCount; i++)
		{
		output << ' ' << *(fontList.GetElement(i));
		}

	// write list of rgb values

	const JSize colorCount = colorList.GetElementCount();
	output << ' ' << colorCount;

	for (i=1; i<=colorCount; i++)
		{
		output << ' ' << colormap->GetRGB(colorList.GetElement(i));
		}

	// write styles

	output << ' ' << styleRunCount;

	i          = charRange.first;
	runIndex   = startRunIndex;
	firstInRun = startFirstInRun;
	do
		{
		JSize charCount = style.GetRunLength(runIndex) - (i - firstInRun);
		if (charRange.last < i + charCount - 1)
			{
			charCount = charRange.last - i + 1;
			}

		const JFont& f   = style.GetRunDataRef(runIndex);
		JString fontName = f.GetName();

		JIndex fontIndex;
		found = fontList.SearchSorted(&fontName, JListT::kAnyMatch, &fontIndex);
		assert( found );

		const JFontStyle& fStyle = f.GetStyle();

		JIndex colorIndex;
		found = colorList.SearchSorted(fStyle.color, JListT::kAnyMatch, &colorIndex);
		assert( found );

		output << ' ' << charCount;
		output << ' ' << fontIndex;
		output << ' ' << f.GetSize();
		output << ' ' << fStyle.bold << fStyle.italic << fStyle.strike;
		output << ' ' << fStyle.underlineCount;
		output << ' ' << colorIndex;

		i += charCount;
		runIndex++;
		firstInRun = i;
		}
		while (i <= charRange.last);
}

/******************************************************************************
 Search and replace

	We only support regular expressions because there is no advantage in
	optimizing for literal strings.

 ******************************************************************************/

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchForward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	JBoolean*		wrapped
	)
{
	const TextIndex i(
		!itsCharSelection.IsEmpty() ? itsCharSelection.last + 1 :
		itsCaretLoc.charIndex,

		!itsByteSelection.IsEmpty() ? itsByteSelection.last + 1 :
		itsCaretLoc.byteIndex);

	const JStringMatch m =
		SearchForward(itsBuffer, i, regex,
					  entireWord, wrapSearch, wrapped);
	if (!m.IsEmpty())
		{
		SetSelection(m.GetCharacterRange(), m.GetUtf8ByteRange());
		}

	return m;
}

// private

JStringMatch
JTextEditor::SearchForward
	(
	const JString&		buffer,
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	TextIndex i = startIndex;

	*wrapped = kJFalse;
	if (i.charIndex > buffer.GetCharacterCount() && wrapSearch)
		{
		i.charIndex = 1;
		i.byteIndex = 1;
		*wrapped    = kJTrue;
		}
	else if (i.charIndex > buffer.GetCharacterCount())
		{
		return JStringMatch(buffer);
		}

	JStringIterator iter(buffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, i.charIndex, i.byteIndex);

	while (1)
		{
		if (iter.Next(regex))
			{
			const JStringMatch m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(buffer, m.GetCharacterRange(), m.GetUtf8ByteRange()))
				{
				return m;
				}
			}

		if (iter.AtEnd() && wrapSearch && !(*wrapped) && startIndex.charIndex > 1)
			{
			iter.MoveTo(kJIteratorStartAtBeginning, 0);
			*wrapped  = kJTrue;
			}
		else if (iter.AtEnd() || (*wrapped && iter.GetNextCharacterIndex() >= startIndex.charIndex))
			{
			break;
			}
		}

	return JStringMatch(buffer);
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchBackward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	JBoolean*		wrapped
	)
{
	TextIndex i;
	if (itsCharSelection.IsEmpty())
		{
		i.charIndex = itsCaretLoc.charIndex;
		i.byteIndex = itsCaretLoc.byteIndex;
		}
	else
		{
		i.charIndex = itsCharSelection.first;
		i.byteIndex = itsByteSelection.first;
		}

	const JStringMatch m =
		SearchBackward(itsBuffer, i, regex,
					   entireWord, wrapSearch, wrapped);
	if (!m.IsEmpty())
		{
		SetSelection(m.GetCharacterRange(), m.GetUtf8ByteRange());
		}

	return m;
}

// private

JStringMatch
JTextEditor::SearchBackward
	(
	const JString&		buffer,
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	TextIndex i = startIndex;

	*wrapped = kJFalse;
	if (i.charIndex == 1 && wrapSearch)
		{
		i.charIndex = buffer.GetCharacterCount();
		i.byteIndex = buffer.GetByteCount();
		*wrapped  = kJTrue;
		}
	else if (i.charIndex == 1)
		{
		return JStringMatch(buffer);
		}

	JStringIterator iter(buffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, i.charIndex, i.byteIndex);

	while (1)
		{
		if (iter.Prev(regex))
			{
			const JStringMatch m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(buffer, m.GetCharacterRange(), m.GetUtf8ByteRange()))
				{
				return m;
				}
			}

		if (iter.AtBeginning() && wrapSearch && !(*wrapped) &&
			startIndex.charIndex < buffer.GetCharacterCount())
			{
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			*wrapped = kJTrue;
			}
		else if (iter.AtBeginning() ||
				  (*wrapped && iter.GetPrevCharacterIndex() <= startIndex.charIndex))
			{
			break;
			}
		}

	return JStringMatch(buffer);
}

/******************************************************************************
 SelectionMatches

	Returns kJTrue if the current selection matches the given search criteria.
	This returns the JStringMatch required for a replace.

 ******************************************************************************/

JStringMatch
JTextEditor::SelectionMatches
	(
	const JRegex&	regex,
	const JBoolean	entireWord
	)
{
	if (itsCharSelection.IsEmpty() ||
		(entireWord && !IsEntireWord(itsBuffer, itsCharSelection, itsByteSelection)))
		{
		return JStringMatch(itsBuffer);
		}

	// We cannot match only the selected text, because that will fail if
	// there are look-behind or look-ahead assertions.

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, itsCharSelection.first, itsByteSelection.first);
	if (iter.Next(regex))
		{
		const JStringMatch m = iter.GetLastMatch();
		if (m.GetCharacterRange() == itsCharSelection)
			{
			return m;
			}
		}

	return JStringMatch(itsBuffer);
}

/******************************************************************************
 IsEntireWord (private)

	Return kJTrue if the given character range is a single, complete word.

 ******************************************************************************/

JBoolean
JTextEditor::IsEntireWord
	(
	const JString&			buffer,
	const JCharacterRange&	charRange,
	const JUtf8ByteRange&	byteRange
	)
	const
{
	JStringIterator iter(buffer);
	JUtf8Character c;

	if (charRange.first > 1)
		{
		iter.UnsafeMoveTo(kJIteratorStartBefore, charRange.first, byteRange.first);
		if (iter.Prev(&c) && IsCharacterInWord(c))
			{
			return kJFalse;
			}
		}

	if (charRange.last < buffer.GetCharacterCount())
		{
		iter.UnsafeMoveTo(kJIteratorStartAfter, charRange.last, byteRange.last);
		if (iter.Next(&c) && IsCharacterInWord(c))
			{
			return kJFalse;
			}
		}

	iter.UnsafeMoveTo(kJIteratorStartBefore, charRange.first, byteRange.first);
	while (iter.Next(&c))
		{
		if (!IsCharacterInWord(c))
			{
			return kJFalse;
			}

		if (iter.GetNextCharacterIndex() > charRange.last)
			{
			break;
			}
		}

	return kJTrue;
}

/******************************************************************************
 ReplaceSelection (protected)

	Replace the current selection with the given replace text.

	*** Something must be selected.
		We do not check that it matches the search string.

 ******************************************************************************/

void
JTextEditor::ReplaceSelection
	(
	const JStringMatch&	match,
	const JString&		replaceStr,
	const JBoolean		replaceIsRegex,
	const JBoolean		preserveCase
	)
{
	assert( HasSelection() );

	JString replaceText;
	if (replaceIsRegex)
		{
		replaceText = itsInterpolator.Interpolate(replaceStr, match);
		}
	else
		{
		replaceText = replaceStr;
		}

	if (preserveCase)
		{
		replaceText.MatchCase(itsBuffer.GetBytes(), match.GetUtf8ByteRange());
		}

	JIndex charIndex, byteIndex;
	if (!itsCharSelection.IsEmpty())
		{
		charIndex = itsCharSelection.first;
		byteIndex = itsByteSelection.first;
		}
	else
		{
		charIndex = itsCaretLoc.charIndex;
		byteIndex = itsCaretLoc.byteIndex;
		}

	Paste(replaceText);
	if (!replaceText.IsEmpty())
		{
		SetSelection(JCharacterRange(charIndex, charIndex + replaceText.GetCharacterCount()-1),
					 JUtf8ByteRange(byteIndex,  byteIndex + replaceText.GetByteCount()-1));
		}
}

/******************************************************************************
 ReplaceAllForward

	Replace every occurrence of the search pattern with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JTextEditor::ReplaceAllForward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	const JString&	replaceStr,
	const JBoolean	replaceIsRegex,
	const JBoolean	preserveCase,
	const JBoolean	restrictToSelection
	)
{
	const TextIndex i = GetInsertionIndex();

	JString buffer;
	JRunArray<JFont> styles;
	if (restrictToSelection && !HasSelection())
		{
		return kJFalse;
		}
	else if (restrictToSelection)
		{
		Copy(&buffer, &styles);
		}
	else if (wrapSearch)
		{
		SelectAll();
		buffer = itsBuffer;
		styles = *itsStyles;
		}
	else if (i.charIndex <= itsBuffer.GetCharacterCount())
		{
		SetSelection(JCharacterRange(i.charIndex, itsBuffer.GetCharacterCount()),
					 JUtf8ByteRange(i.byteIndex, itsBuffer.GetByteCount()));
		Copy(&buffer, &styles);
		}
	else
		{
		return kJFalse;
		}

	JLatentPG pg(100);
	pg.VariableLengthProcessBeginning(JGetString("ReplacingText::JTextEditor"), kJTrue, kJFalse);

	JBoolean foundAny = kJFalse;

	JStringIterator iter(&buffer);
	while (iter.Next(regex))
		{
		const JStringMatch m = iter.GetLastMatch();
		if (!entireWord || IsEntireWord(buffer, m.GetCharacterRange(), m.GetUtf8ByteRange()))
			{
			ReplaceRange(&iter, &styles, m, replaceStr, replaceIsRegex, preserveCase);
			foundAny = kJTrue;

			if (!pg.IncrementProgress())
				{
				break;
				}
			}
		}

	pg.ProcessFinished();

	if (foundAny)
	{
		const JIndex charIndex = itsCharSelection.first,
					 byteIndex = itsByteSelection.first;

		Paste(buffer, &styles);

		if (restrictToSelection)
			{
			SetSelection(JCharacterRange(charIndex, charIndex + buffer.GetCharacterCount()-1),
						 JUtf8ByteRange(byteIndex, byteIndex + buffer.GetByteCount()-1));
			}
	}

	return foundAny;
}

/******************************************************************************
 ReplaceAllBackward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JTextEditor::ReplaceAllBackward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	const JString&	replaceStr,
	const JBoolean	replaceIsRegex,
	const JBoolean	preserveCase
	)
{
	const TextIndex i = GetInsertionIndex();

	JString buffer;
	JRunArray<JFont> styles;

	JString selText;
	JIndexRange matchRange;
	if (wrapSearch)
		{
		SelectAll();
		buffer = itsBuffer;
		styles = *itsStyles;
		}
	else if (!SelectionMatches(regex, entireWord).IsEmpty())
		{
		SetSelection(JCharacterRange(1, itsCharSelection.last),
					 JUtf8ByteRange(1, itsByteSelection.last));
		Copy(&buffer, &styles);
		}
	else if (i.charIndex > 1)
		{
		SetSelection(JCharacterRange(1, i.charIndex-1),
					 JUtf8ByteRange(1, i.byteIndex-1));
		Copy(&buffer, &styles);
		}
	else
		{
		return kJFalse;
		}

	JLatentPG pg(10);	// MatchBackward() can be slow
	pg.VariableLengthProcessBeginning(JGetString("ReplacingText::JTextEditor"), kJTrue, kJFalse);

	JBoolean foundAny = kJFalse;

	JStringIterator iter(&buffer, kJIteratorStartAtEnd);
	while (iter.Prev(regex))
		{
		const JStringMatch m = iter.GetLastMatch();
		if (!entireWord || IsEntireWord(buffer, m.GetCharacterRange(), m.GetUtf8ByteRange()))
			{
			ReplaceRange(&iter, &styles, m, replaceStr, replaceIsRegex, preserveCase);
			foundAny = kJTrue;

			if (!pg.IncrementProgress())
				{
				break;
				}
			}
		}

	pg.ProcessFinished();

	if (foundAny)
	{
		Paste(buffer, &styles);
		SetCaretLocation(1);
	}

	return foundAny;
}

/******************************************************************************
 ReplaceRange (private)

	Replace the specified range with the given replace text.

 ******************************************************************************/

void
JTextEditor::ReplaceRange
	(
	JStringIterator*	buffer,
	JRunArray<JFont>*	styles,
	const JStringMatch&	match,
	const JString&		replaceStr,
	const JBoolean		replaceIsRegex,
	const JBoolean		preserveCase
	)
{
	JString replaceText;
	if (replaceIsRegex)
		{
		replaceText = itsInterpolator.Interpolate(replaceStr, match);
		}
	else
		{
		replaceText = replaceStr;
		}

	if (preserveCase)
		{
		replaceText.MatchCase(buffer->GetString().GetRawBytes(), match.GetUtf8ByteRange());
		}

	const JCharacterRange charRange = match.GetCharacterRange();
	const JUtf8ByteRange byteRange  = match.GetUtf8ByteRange();

	buffer->UnsafeMoveTo(kJIteratorStartBefore, charRange.first, byteRange.first);
	buffer->BeginMatch();
	buffer->UnsafeMoveTo(kJIteratorStartAfter, charRange.last, byteRange.last);
	buffer->FinishMatch();
	buffer->RemoveLastMatch();

	styles->RemoveNextElements(charRange.first, charRange.GetCount());

	InsertText(buffer, styles, replaceText, NULL, NULL);
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::SearchForward
	(
	const FontMatch&	match,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	JIndex startIndex =
		HasSelection() ? itsCharSelection.last + 1 : itsCaretLoc.charIndex;

	const JSize runCount = itsStyles->GetRunCount();
	JIndex endRun        = runCount;

	*wrapped = kJFalse;
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex > firstIndexInRun)
				{
				firstIndexInRun += itsStyles->GetRunLength(startRun);
				startRun++;
				}

			for (JIndex i=startRun; i<=endRun; i++)
				{
				const JFont& f        = itsStyles->GetRunDataRef(i);
				const JSize runLength = itsStyles->GetRunLength(i);
				if (match.Match(f))
					{
					const JCharacterRange charRange(firstIndexInRun, firstIndexInRun + runLength-1);

					JUtf8ByteRange byteRange;
					if (wrapped)
						{
						byteRange = JString::CharacterToUtf8ByteRange(itsBuffer.GetRawBytes(), charRange);
						}
					else	// optimize from where we started
						{
						const JSize byteOffset =
							HasSelection() ? itsByteSelection.last : itsCaretLoc.byteIndex - 1;

						byteRange.SetFirstAndCount(
							byteOffset + 1,
							JString::CountBytes(
								itsBuffer.GetRawBytes() + byteOffset,
								charRange.GetCount()));
						}

					SetSelection(charRange, byteRange);
					return kJTrue;
					}

				firstIndexInRun += runLength;
				}

			if (wrapSearch && !(*wrapped) && origStartRun > 1)
				{
				startIndex = 1;
				endRun     = origStartRun-1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = 1;
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::SearchBackward
	(
	const FontMatch&	match,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	JIndex startIndex =
		HasSelection() ? itsCharSelection.first - 1 : itsCaretLoc.charIndex - 1;

	JIndex endRun = 1;

	*wrapped = kJFalse;
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex < firstIndexInRun + itsStyles->GetRunLength(startRun)-1)
				{
				startRun--;
				if (startRun > 0)
					{
					firstIndexInRun -= itsStyles->GetRunLength(startRun);
					}
				}

			for (JIndex i=startRun; i>=endRun; i--)
				{
				const JFont& f = itsStyles->GetRunDataRef(i);
				if (match.Match(f))
					{
					const JCharacterRange charRange(firstIndexInRun, firstIndexInRun + itsStyles->GetRunLength(i)-1);

					JUtf8ByteRange byteRange;
					if (wrapped)
						{
						byteRange = JString::CharacterToUtf8ByteRange(itsBuffer.GetRawBytes(), charRange);
						}
					else	// optimize from where we started
						{
						const JSize byteIndex =
							HasSelection() ? itsByteSelection.first : itsCaretLoc.byteIndex;

						JSize byteCount;
						const JBoolean ok =
							JString::CountBytesBackward(itsBuffer.GetRawBytes(), byteIndex - 1,
														charRange.GetCount(), &byteCount);
						assert(ok);

						byteRange.Set(byteIndex - byteCount, byteIndex - 1);
						}

					SetSelection(charRange, byteRange);
					return kJTrue;
					}

				if (i > endRun)
					{
					firstIndexInRun -= itsStyles->GetRunLength(i-1);
					}
				}

			if (wrapSearch && !(*wrapped) && origStartRun < itsStyles->GetRunCount())
				{
				startIndex = itsStyles->GetElementCount();
				endRun     = origStartRun+1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = itsStyles->GetElementCount();
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 FontMatch destructor

 ******************************************************************************/

JTextEditor::FontMatch::~FontMatch()
{
}

/******************************************************************************
 SetBreakCROnly

 ******************************************************************************/

void
JTextEditor::SetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	if (breakCROnly != itsBreakCROnlyFlag)
		{
		PrivateSetBreakCROnly(breakCROnly);
		RecalcAll(kJFalse);
		}
}

// private

void
JTextEditor::PrivateSetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	itsBreakCROnlyFlag = breakCROnly;
	if (!itsBreakCROnlyFlag)
		{
		itsWidth = itsGUIWidth;
		}
}

/******************************************************************************
 TEGetMinPreferredGUIWidth

 ******************************************************************************/

JCoordinate
JTextEditor::TEGetMinPreferredGUIWidth()
	const
{
	if (itsBreakCROnlyFlag)
		{
		return itsLeftMarginWidth + itsWidth + kRightMarginWidth;
		}
	else
		{
		return itsLeftMarginWidth + itsMaxWordWidth + kRightMarginWidth;
		}
}

/******************************************************************************
 TESetBoundsWidth (protected)

	Call this to notify us of a change in aperture width.

 ******************************************************************************/

void
JTextEditor::TESetBoundsWidth
	(
	const JCoordinate width
	)
{
	itsGUIWidth = width - itsLeftMarginWidth - kRightMarginWidth;
	assert( itsGUIWidth > 0 );
	TEGUIWidthChanged();
}

/******************************************************************************
 TESetLeftMarginWidth

	For multi-line input areas it helps to have a wide left margin where
	one can click and drag down to select entire lines.

 ******************************************************************************/

void
JTextEditor::TESetLeftMarginWidth
	(
	const JCoordinate origWidth
	)
{
	const JCoordinate width = JMax((JCoordinate) kMinLeftMarginWidth, origWidth);

	if (width != itsLeftMarginWidth)
		{
		itsGUIWidth       += itsLeftMarginWidth;
		itsLeftMarginWidth = width;
		itsGUIWidth       -= itsLeftMarginWidth;
		TEGUIWidthChanged();
		}
}

/******************************************************************************
 TEGUIWidthChanged (private)

 ******************************************************************************/

void
JTextEditor::TEGUIWidthChanged()
{
	if (itsBreakCROnlyFlag)
		{
		TERefresh();
		}
	else if (itsWidth != itsGUIWidth)
		{
		itsWidth = itsGUIWidth;
		RecalcAll(kJFalse);
		}
}

/******************************************************************************
 GetCurrentFont

 ******************************************************************************/

JFont
JTextEditor::GetCurrentFont()
	const
{
	if (!itsCharSelection.IsEmpty())
		{
		return itsStyles->GetElement(itsCharSelection.first);
		}
	else
		{
		return itsInsertionFont;
		}
}

/******************************************************************************
 Set current font

 ******************************************************************************/

void
JTextEditor::SetCurrentFontName
	(
	const JString& name
	)
{
	if (!itsCharSelection.IsEmpty())
		{
		JBoolean isNew;
		JTEUndoStyle* undo = GetStyleUndo(&isNew);

		const JBoolean changed =
			SetFontName(itsCharSelection.first, itsCharSelection.last, name, kJFalse);

		if (changed)
			{
			NewUndo(undo, isNew);
			}
		else if (isNew)
			{
			jdelete undo;
			}
		}
	else
		{
		itsInsertionFont.SetName(name);
		}
}

void
JTextEditor::SetCurrentFontSize
	(
	const JSize size
	)
{
	#define LocalVarName   size
	#define GetElementName GetSize()
	#define SetElementName SetSize
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontBold
	(
	const JBoolean bold
	)
{
	#define LocalVarName   bold
	#define GetElementName GetStyle().bold
	#define SetElementName SetBold
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontItalic
	(
	const JBoolean italic
	)
{
	#define LocalVarName   italic
	#define GetElementName GetStyle().italic
	#define SetElementName SetItalic
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontUnderline
	(
	const JSize count
	)
{
	#define LocalVarName   count
	#define GetElementName GetStyle().underlineCount
	#define SetElementName SetUnderlineCount
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontStrike
	(
	const JBoolean strike
	)
{
	#define LocalVarName   strike
	#define GetElementName GetStyle().strike
	#define SetElementName SetStrike
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontColor
	(
	const JColorIndex color
	)
{
	#define LocalVarName   color
	#define GetElementName GetStyle().color
	#define SetElementName SetColor
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFontStyle
	(
	const JFontStyle& style
	)
{
	#define LocalVarName   style
	#define GetElementName GetStyle()
	#define SetElementName SetStyle
	#include <JTESetCurrentFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetCurrentFont
	(
	const JFont& f
	)
{
	if (!itsCharSelection.IsEmpty())
		{
		JBoolean isNew;
		JTEUndoStyle* undo = GetStyleUndo(&isNew);
		SetFont(itsCharSelection.first, itsCharSelection.last, f, kJFalse);
		NewUndo(undo, isNew);
		}
	else
		{
		itsInsertionFont = f;
		}
}

/******************************************************************************
 Set font

	Returns kJTrue if anything actually changed

 ******************************************************************************/

JBoolean
JTextEditor::SetFontName
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JString&	name,
	const JBoolean	clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	JFont f1 = itsFontMgr->GetDefaultFont(), f2 = itsFontMgr->GetDefaultFont();
	JBoolean changed = kJFalse;
	JRunArrayIterator<JFont> iter(itsStyles, kJIteratorStartBefore, startIndex);
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		const JBoolean ok = iter.Next(&f1);
		assert( ok );

		f2 = f1;
		f2.SetName(name);
		if (f2 != f1)
			{
			iter.SetPrev(f2);
			changed = kJTrue;
			}
		}

	if (changed)
		{
		const JUtf8ByteRange r =
			JString::CharacterToUtf8ByteRange(
				itsBuffer.GetBytes(),
				JCharacterRange(startIndex, startIndex));

		Recalc(TextIndex(startIndex, r.first),
			   endIndex - startIndex + 1, kJFalse);
		}

	return changed;
}

JBoolean
JTextEditor::SetFontSize
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		size,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   size
	#define GetElementName GetSize()
	#define SetElementName SetSize
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontBold
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	bold,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   bold
	#define GetElementName GetStyle().bold
	#define SetElementName SetBold
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontItalic
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	italic,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   italic
	#define GetElementName GetStyle().italic
	#define SetElementName SetItalic
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontUnderline
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		count,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   count
	#define GetElementName GetStyle().underlineCount
	#define SetElementName SetUnderlineCount
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontStrike
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	strike,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   strike
	#define GetElementName GetStyle().strike
	#define SetElementName SetStrike
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontColor
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JColorIndex	color,
	const JBoolean		clearUndo

	)
{
	#define LocalVarName   color
	#define GetElementName GetStyle().color
	#define SetElementName SetColor
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JTextEditor::SetFontStyle
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JFontStyle&	style,
	const JBoolean		clearUndo
	)
{
	#define LocalVarName   style
	#define GetElementName GetStyle()
	#define SetElementName SetStyle
	#include <JTESetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JTextEditor::SetFont
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JFont&		f,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	const JUtf8ByteRange r =
		JString::CharacterToUtf8ByteRange(
			itsBuffer.GetBytes(),
			JCharacterRange(startIndex, startIndex));

	if (endIndex > startIndex)
		{
		const JSize charCount = endIndex - startIndex + 1;
		itsStyles->SetNextElements(startIndex, charCount, f);
		Recalc(TextIndex(startIndex, r.first), charCount, kJFalse);
		}
	else
		{
		assert( startIndex == endIndex );

		itsStyles->SetElement(startIndex, f);
		Recalc(TextIndex(startIndex, r.first), 1, kJFalse);
		}
}

// protected

void
JTextEditor::SetFont
	(
	const JIndex			startIndex,
	const JRunArray<JFont>&	fontList,
	const JBoolean			clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	JFont f = itsFontMgr->GetDefaultFont();
	JRunArrayIterator<JFont> fIter(fontList);
	JRunArrayIterator<JFont> sIter(itsStyles, kJIteratorStartBefore, startIndex);

	while (fIter.Next(&f) && sIter.SetNext(f))
		{
		sIter.SkipNext();
		}

	const JUtf8ByteRange r =
		JString::CharacterToUtf8ByteRange(
			itsBuffer.GetBytes(),
			JCharacterRange(startIndex, startIndex));

	Recalc(TextIndex(startIndex, r.first), fontList.GetElementCount(), kJFalse);
}

/******************************************************************************
 SetAllFontNameAndSize (protected)

	This function is useful for unstyled text editors that allow the user
	to change the font and size.

	It preserves the styles, in case they are controlled by the program.
	(e.g. context sensitive hilighting)

	You can choose whether or not to throw out all Undo information.
	Unstyled text editors can usually preserve Undo, since they will not
	allow the user to modify styles.  (We explicitly ask for this because
	it is too easy to forget about the issue.)

 ******************************************************************************/

void
JTextEditor::SetAllFontNameAndSize
	(
	const JString&		name,
	const JSize			size,
	const JCoordinate	tabWidth,
	const JBoolean		breakCROnly,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	const JSize runCount = itsStyles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JFont f = itsStyles->GetRunData(i);
		f.Set(name, size, f.GetStyle());
		itsStyles->SetRunData(i, f);
		}

	if (itsUndoList != NULL)
		{
		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=1; i<=undoCount; i++)
			{
			(itsUndoList->GetElement(i))->SetFont(name, size);
			}
		}
	else if (itsUndo != NULL)
		{
		itsUndo->SetFont(name, size);
		}

	itsInsertionFont.Set(name, size, itsInsertionFont.GetStyle());
	itsDefFont.Set(name, size, itsDefFont.GetStyle());

	itsDefTabWidth = tabWidth;
	PrivateSetBreakCROnly(breakCROnly);
	RecalcAll(kJFalse);
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JTextEditor::SetDefaultFontName
	(
	const JString& name
	)
{
	itsDefFont.SetName(name);
	if (itsBuffer.IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(TextIndex(1,1));
		RecalcAll(kJFalse);
		}
}

void
JTextEditor::SetDefaultFontSize
	(
	const JSize size
	)
{
	itsDefFont.SetSize(size);
	if (itsBuffer.IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(TextIndex(1,1));
		RecalcAll(kJFalse);
		}
}

void
JTextEditor::SetDefaultFontStyle
	(
	const JFontStyle& style
	)
{
	itsDefFont.SetStyle(style);
	if (itsBuffer.IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(TextIndex(1,1));
		RecalcAll(kJFalse);
		}
}

void
JTextEditor::SetDefaultFont
	(
	const JFont& font
	)
{
	itsDefFont = font;
	if (itsBuffer.IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(TextIndex(1,1));
		RecalcAll(kJFalse);
		}
}

/******************************************************************************
 Cut

 ******************************************************************************/

void
JTextEditor::Cut()
{
	if (!TEIsDragging())
		{
		TEClipboardChanged();
		DeleteSelection();
		DeactivateCurrentUndo();
		}
}

/******************************************************************************
 Cut

	Returns kJTrue if there was anything to cut.  style can be NULL.
	If function returns kJFalse, text and style are not modified.

 ******************************************************************************/

JBoolean
JTextEditor::Cut
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	if (!TEIsDragging() && Copy(text, style))
		{
		DeleteSelection();
		DeactivateCurrentUndo();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JTextEditor::Copy()
{
	TEClipboardChanged();
}

/******************************************************************************
 Copy

	Returns kJTrue if there was anything to copy.  style can be NULL.
	If function returns kJFalse, text and style are not modified, so
	we cannot call GetSelection().

 ******************************************************************************/

JBoolean
JTextEditor::Copy
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	if (!itsCharSelection.IsEmpty())
		{
		text->Set(itsBuffer.GetRawBytes(), itsByteSelection);
		style->RemoveAll();
		style->AppendSlice(*itsStyles, itsCharSelection);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetClipboard

	Returns the text and style that would be pasted if Paste() were called.

	style can be NULL.  If it is not NULL, it can come back empty even if
	the function returns kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::GetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	JRunArray<JFont> tempStyle;
	return TEGetExternalClipboard(text, &tempStyle);
}

/******************************************************************************
 Paste

 ******************************************************************************/

void
JTextEditor::Paste()
{
	JString text;
	JRunArray<JFont> style;
	if (TEGetExternalClipboard(&text, &style))
		{
		JRunArray<JFont>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);
		}
}

/******************************************************************************
 Paste

	style can be NULL

 ******************************************************************************/

void
JTextEditor::Paste
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	assert( style == NULL || style->GetElementCount() == text.GetCharacterCount() );

	if (itsIsDragSourceFlag)
		{
		return;
		}

	JTEUndoPaste* newUndo = jnew JTEUndoPaste(this, TextCount(text.GetCharacterCount(), text.GetByteCount()));
	assert( newUndo != NULL );

	const TextCount pasteLength = PrivatePaste(text, style);
	newUndo->SetPasteLength(pasteLength);

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 PrivatePaste (private)

	Returns number of characters that were actually inserted.

	style can be NULL.

 ******************************************************************************/

JTextEditor::TextCount
JTextEditor::PrivatePaste
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	const JBoolean hadSelection = HasSelection();
	if (hadSelection)
		{
		itsInsertionFont = itsStyles->GetElement(itsCharSelection.first);
		DeleteText(itsCharSelection, itsByteSelection);
		SetCaretLocation(CalcCaretLocation(
			TextIndex(itsCharSelection.first, itsByteSelection.first)));
		itsCharSelection.SetToNothing();
		itsByteSelection.SetToNothing();
		}

	TextCount pasteLength;
	if (itsPasteStyledTextFlag)
		{
		pasteLength = InsertText(itsCaretLoc, text, style);
		}
	else
		{
		pasteLength = InsertText(itsCaretLoc, text);
		}

	Recalc(itsCaretLoc, pasteLength.charCount, hadSelection, kJFalse);
	SetCaretLocation(CalcCaretLocation(
		TextIndex(itsCharSelection.first + pasteLength.charCount,
				  itsByteSelection.first + pasteLength.byteCount)));
	return pasteLength;
}

/******************************************************************************
 GetInsertionIndex (protected)

	Return the index where new text will be typed or pasted.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetInsertionIndex()
	const
{
	if (!itsCharSelection.IsEmpty())
		{
		return TextIndex(itsCharSelection.first, itsByteSelection.first);
		}
	else
		{
		return TextIndex(itsCaretLoc);
		}
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

JBoolean
JTextEditor::GetSelection
	(
	JString* text
	)
	const
{
	if (itsCharSelection.IsEmpty())
		{
		text->Clear();
		return kJFalse;
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, itsCharSelection.first, itsByteSelection.first);
	iter.BeginMatch();
	iter.UnsafeMoveTo(kJIteratorStartAfter, itsCharSelection.last, itsByteSelection.last);
	*text = iter.FinishMatch().GetString();
	return kJTrue;
}

JBoolean
JTextEditor::GetSelection
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	style->RemoveAll();

	if (!itsCharSelection.IsEmpty())
		{
		GetSelection(text);
		style->InsertElementsAtIndex(1, *itsStyles, itsCharSelection.first,
									 itsCharSelection.GetCount());
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetSelection (private)

	*** Requires, but does not validate, that charRange and byteRange
		correspond to the same bytes.

 ******************************************************************************/

void
JTextEditor::SetSelection
	(
	const JCharacterRange&	charRange,
	const JUtf8ByteRange&	byteRange,
	const JBoolean			needCaretBcast
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	DeactivateCurrentUndo();
	itsPrevDragType = kInvalidDrag;		// avoid wordSel and lineSel pivots

	if (itsBuffer.IsEmpty() || itsCharSelection == charRange)
		{
		return;
		}

	assert( itsBuffer.CharacterIndexValid(charRange.first) );
	assert( itsBuffer.CharacterIndexValid(charRange.last) );
	assert( !charRange.IsEmpty() );

	assert( itsBuffer.ByteIndexValid(byteRange.first) );
	assert( itsBuffer.ByteIndexValid(byteRange.last) );
	assert( !byteRange.IsEmpty() );

	const JBoolean hadSelection            = !itsCharSelection.IsEmpty();
	const CaretLocation origCaretLoc       = itsCaretLoc;
	const JUtf8ByteRange origByteSelection = itsByteSelection;

	itsCaretLoc      = CaretLocation(0,0,0);
	itsCharSelection = charRange;
	itsByteSelection = byteRange;

	const JIndex newStartLine = GetLineForByte(itsByteSelection.first);
	const JIndex newEndLine   = GetLineForByte(itsByteSelection.last);

	if (needCaretBcast)
		{
		BroadcastCaretMessages(CaretLocation(itsCharSelection.first, itsByteSelection.first, newStartLine), kJTrue);
		}

	TECaretShouldBlink(kJFalse);

	if (theCopyWhenSelectFlag && itsType != kStaticText)
		{
		Copy();
		}

	// We only optimize heavily for the case when one end of the
	// selection remains fixed because this is the case during mouse drags.

	if (hadSelection && origByteSelection.first == itsByteSelection.first)
		{
		const JIndex origEndLine = GetLineForByte(origByteSelection.last);
		TERefreshLines(JMin(origEndLine, newEndLine),
					   JMax(origEndLine, newEndLine));
		}
	else if (hadSelection && origByteSelection.last == itsByteSelection.last)
		{
		const JIndex origStartLine = GetLineForByte(origByteSelection.first);
		TERefreshLines(JMin(origStartLine, newStartLine),
					   JMax(origStartLine, newStartLine));
		}
	else if (hadSelection)
		{
		const JIndex origStartLine = GetLineForByte(origByteSelection.first);
		const JIndex origEndLine   = GetLineForByte(origByteSelection.last);
		TERefreshLines(origStartLine, origEndLine);
		TERefreshLines(newStartLine, newEndLine);
		}
	else
		{
		TERefreshCaret(origCaretLoc);
		TERefreshLines(newStartLine, newEndLine);
		}
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JTextEditor::SelectAll()
{
	if (!itsBuffer.IsEmpty())
		{
		SetSelection(JCharacterRange(1, itsBuffer.GetCharacterCount()),
					 JUtf8ByteRange( 1, itsBuffer.GetByteCount()));
		}
}

/******************************************************************************
 DeleteSelection

	We create JTEUndoTyping so keys pressed after the delete key count
	as part of the undo task.

 ******************************************************************************/

void
JTextEditor::DeleteSelection()
{
	if (!itsIsDragSourceFlag && !itsCharSelection.IsEmpty())
		{
		JTEUndoTyping* newUndo = jnew JTEUndoTyping(this);
		assert( newUndo != NULL );

		DeleteText(itsCharSelection, itsByteSelection);
		Recalc(TextIndex(itsCharSelection.first, itsByteSelection.first), 1, kJTrue, kJFalse);
		SetCaretLocation(CalcCaretLocation(
			TextIndex(itsCharSelection.first, itsByteSelection.first)));

		NewUndo(newUndo, kJTrue);
		}
}

/******************************************************************************
 TabSelectionLeft

	Remove tabs from the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor::TabSelectionLeft
	(
	const JSize		tabCount,
	const JBoolean	force
	)
{
	if (TEIsDragging())
		{
		return;
		}

	// for undo

	if (!itsCharSelection.IsEmpty())
		{
		const TextIndex start = GetParagraphStart(TextIndex(itsCharSelection.first, itsByteSelection.first)),
						end   = GetParagraphEnd(TextIndex(itsCharSelection.last, itsByteSelection.last));
		SetSelection(JCharacterRange(start.charIndex, end.charIndex),
					 JUtf8ByteRange(start.byteIndex, end.byteIndex));
		}
	else if (!itsBuffer.IsEmpty())
		{
		const TextIndex i     = TextIndex(itsCaretLoc.charIndex, itsCaretLoc.byteIndex),
						start = GetParagraphStart(i),
						end   = GetParagraphEnd(i);
		SetSelection(JCharacterRange(start.charIndex, end.charIndex),
					 JUtf8ByteRange(start.byteIndex, end.byteIndex));
		}
	else
		{
		return;
		}

	const JIndex firstLine = GetLineForChar(itsCharSelection.first);
	const JIndex lastLine  = GetLineForChar(itsCharSelection.last);

	// check that there are enough tabs at the start of every selected line,
	// ignoring lines created by wrapping

	JBoolean sufficientWS      = kJTrue;
	JSize prefixSpaceCount     = 0;		// min # of spaces at start of line
	JBoolean firstNonemptyLine = kJTrue;

	JStringIterator iter(&itsBuffer);
	for (JIndex i=firstLine; i<=lastLine; i++)
		{
		const TextIndex firstChar = GetLineStart(i);
		iter.UnsafeMoveTo(kJIteratorStartBeforeByte, firstChar.charIndex, firstChar.byteIndex);

		JUtf8Character c;
		if ((!iter.Prev(&c, kJFalse) || c == '\n') &&
			iter.Next(&c, kJFalse) && c != '\n')
			{
			for (JIndex j=1; j<=tabCount; j++)
				{
				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (iter.Next(&c) && c == ' ' &&
					   spaceCount < itsCRMTabCharCount)
					{
					spaceCount++;
					}

				if (iter.AtEnd())
					{
					break;	// last line contains only whitespace
					}
				if (j == 1 && (spaceCount < prefixSpaceCount || firstNonemptyLine))
					{
					prefixSpaceCount  = spaceCount;
					firstNonemptyLine = kJFalse;
					}
				if (spaceCount >= itsCRMTabCharCount)
					{
					continue;
					}

				if (c != '\t' && c != '\n')
					{
					sufficientWS = kJFalse;
					break;
					}
				if (c == '\n')
					{
					break;	// line is blank or contains only whitespace
					}
				}
			}
		}

	// If same number of spaces at front of every line, remove those even if
	// not enough to shift by one tab.

	JSize tabCharCount = itsCRMTabCharCount;
	if (!sufficientWS && prefixSpaceCount > 0 && tabCount == 1)
		{
		tabCharCount = prefixSpaceCount;
		}
	else if (force)
		{
		sufficientWS = kJTrue;
		}
	else if (!sufficientWS)
		{
		return;
		}

	JBoolean isNew;
	JTEUndoTabShift* undo = GetTabShiftUndo(&isNew);

	itsCharSelection.SetToNothing();
	itsByteSelection.SetToNothing();
	JSize deleteCount = 0;	// only deleting single-byte characters
	for (JIndex i=firstLine; i<=lastLine; i++)
		{
		const TextIndex firstChar = GetLineStart(i);
		iter.UnsafeMoveTo(kJIteratorStartBeforeByte, firstChar.charIndex, firstChar.byteIndex);
		iter.SkipPrev(deleteCount);

		JUtf8Character c;
		if ((!iter.Prev(&c, kJFalse) || c == '\n') &&
			iter.Next(&c, kJFalse) && c != '\n')
			{
			for (JIndex j=1; j<=tabCount; j++)
				{
				// The deletion point stays in same place (charIndex) and
				// simply eats characters.

				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (iter.Next(&c) && c == ' ' &&
					   spaceCount < tabCharCount)
					{
					spaceCount++;
					DeleteText(&iter, 1);
					deleteCount++;
					}

				if (iter.AtEnd())
					{
					break;	// last line contains only whitespace
					}
				if (!sufficientWS)
					{
					break;		// only remove tabCharCount spaces at front of every line
					}
				if (spaceCount >= tabCharCount)
					{
					continue;	// removed equivalent of one tab
					}

				if (c != '\t')
					{
					break;
					}
				DeleteText(&iter, 1);
				deleteCount++;
				}
			}
		}

	const TextIndex startIndex = GetLineStart(firstLine);
	const TextIndex endIndex   = GetLineEnd(lastLine);
	Recalc(startIndex, endIndex.charIndex - startIndex.charIndex + 1, kJTrue, kJFalse);

	SetSelection(JCharacterRange(startIndex.charIndex, endIndex.charIndex - deleteCount),
				 JUtf8ByteRange(startIndex.byteIndex, endIndex.byteIndex - deleteCount));
	undo->UpdateEndChar();

	NewUndo(undo, isNew);
}

/******************************************************************************
 TabSelectionRight

	Insert a tab at the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor::TabSelectionRight
	(
	const JSize tabCount
	)
{
	if (TEIsDragging())
		{
		return;
		}

JIndex i;

	if (!itsSelection.IsEmpty())
		{
		SetSelection(GetParagraphStart(itsSelection.first),
					 GetParagraphEnd(itsSelection.last));
		}
	else if (!itsBuffer->IsEmpty())
		{
		SetSelection(GetParagraphStart(itsCaretLoc.charIndex),
					 GetParagraphEnd(itsCaretLoc.charIndex));
		}
	else
		{
		return;
		}

	const JIndex firstLine = GetLineForChar(itsSelection.first);
	const JIndex lastLine  = GetLineForChar(itsSelection.last);

	JBoolean isNew;
	JTEUndoTabShift* undo = GetTabShiftUndo(&isNew);

	JString tabs;
	if (itsTabToSpacesFlag)
		{
		const JSize spaceCount = tabCount*itsCRMTabCharCount;
		for (i=1; i<=spaceCount; i++)
			{
			tabs.AppendCharacter(' ');
			}
		}
	else
		{
		for (i=1; i<=tabCount; i++)
			{
			tabs.AppendCharacter('\t');
			}
		}

	itsSelection.SetToNothing();
	JSize insertCount = 0;
	for (i=firstLine; i<=lastLine; i++)
		{
		const JIndex charIndex = GetLineStart(i) + insertCount;
		if ((charIndex == 1 || itsBuffer->GetCharacter(charIndex-1) == '\n') &&
			itsBuffer->GetCharacter(charIndex) != '\n')
			{
			insertCount += InsertText(charIndex, tabs);
			}
		}

	const JIndex startIndex = GetLineStart(firstLine);
	const JIndex endIndex   = GetLineEnd(lastLine) + insertCount;
	Recalc(startIndex, endIndex - startIndex + 1, kJFalse, kJFalse);

	SetSelection(startIndex, endIndex);
	undo->UpdateEndChar();

	NewUndo(undo, isNew);
}

/******************************************************************************
 AnalyzeWhitespace

	Check all indent whitespace.

	If default is tabs, check for indent containing spaces.  Allow up to
	N-1 spaces at the end of the line, where N = tab width.

	If default is spaces, check for any indent containing tabs.

	If mixed indentation, show whitespace.  Set mode based on majority of
	indentation.

	Updates tabWidth value if it decides that spaces should be used for
	indentation.

 ******************************************************************************/

inline JBoolean
isWhitespace
	(
	const JCharacter c
	)
{
	return JI2B( c == ' ' || c == '\t' );
}

void
JTextEditor::AnalyzeWhitespace
	(
	JSize* tabWidth
	)
{
	JBoolean useSpaces, isMixed;
	*tabWidth = AnalyzeWhitespace(*itsBuffer, *tabWidth, itsTabToSpacesFlag,
								  &useSpaces, &isMixed);

	TabShouldInsertSpaces(useSpaces);
	ShouldShowWhitespace(isMixed);
}

/******************************************************************************
 CleanWhitespace

	Clean up the indentation whitespace and strip trailing whitespace in
	the specified range.

 ******************************************************************************/

void
JTextEditor::CleanWhitespace
	(
	const JIndexRange&	range,
	const JBoolean		align
	)
{
	JIndexRange r;
	r.first = GetParagraphStart(range.first);
	r.last  = GetParagraphEnd(range.last);

	SetSelection(r.first, r.last);

	JString text;
	JRunArray<JFont> style;
	const JBoolean ok = Copy(&text, &style);
	assert( ok );

	// strip trailing whitespace -- first, to clear blank lines

	JIndex i = 1;
	JBoolean keepGoing;
	do
		{
		keepGoing = text.LocateNextSubstring("\n", &i);
		JIndex j  = i-1;
		while (i > 1 && isWhitespace(text.GetCharacter(i-1)))
			{
			i--;
			}

		if (i <= j)
			{
			const JSize count = j-i+1;
			text.RemoveSubstring(i, j);
			style.RemoveNextElements(i, count);
			j -= count;
			}

		i = j+2;
		}
		while (keepGoing && i <= text.GetLength());

	// clean indenting whitespace

	i = 0;
	do
		{
		JIndex j = i+1;
		while (i < text.GetLength() && isWhitespace(text.GetCharacter(i+1)))
			{
			i++;
			}

		JSize n = 0;
		JSize p = j;
		if (itsTabToSpacesFlag)
			{
			JString fill;
			for (JIndex k=j; k<=i; k++)
				{
				if (text.GetCharacter(k) == ' ')
					{
					n++;
					if (n == itsCRMTabCharCount)
						{
						n = 0;
						p = k+1;
						}
					}
				else	// tab
					{
					JSize count = itsCRMTabCharCount - n;

					fill.Clear();
					for (JIndex a=1; a<=count; a++)
						{
						fill += " ";
						}

					text.ReplaceSubstring(k, k, fill);
					style.InsertElementsAtIndex(k, style.GetElement(k), count-1);
					k += count-1;
					i += count-1;

					n = 0;
					p = k+1;
					}
				}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
				{
				text.RemoveSubstring(p, i);
				style.RemoveNextElements(p, i-p+1);
				}
			else if (align && itsCRMTabCharCount/2 < n)
				{
				JSize count = itsCRMTabCharCount - n;

				fill.Clear();
				for (JIndex a=1; a<=count; a++)
					{
					fill += " ";
					}

				text.InsertSubstring(fill, p);
				style.InsertElementsAtIndex(p, style.GetElement(p), count);
				}
			}
		else
			{
			for (JIndex k=j; k<=i; k++)
				{
				if (text.GetCharacter(k) == ' ')
					{
					n++;
					if (n == itsCRMTabCharCount)
						{
						const JSize count = k-p;
						text.ReplaceSubstring(p, k, "\t");
						style.RemoveNextElements(p+1, count);
						i -= count;
						k -= count;

						n = 0;
						p = k+1;
						}
					}
				else	// tab
					{
					if (p < k)	// remove useless preceding spaces
						{
						const JSize count = k-p;
						text.RemoveSubstring(p, k-1);
						style.RemoveNextElements(p, count);
						i -= count;
						k -= count;
						}
					n = 0;
					p = k+1;
					}
				}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
				{
				text.RemoveSubstring(p, i);
				style.RemoveNextElements(p, i-p+1);
				}
			else if (align && itsCRMTabCharCount/2 < n)
				{
				text.ReplaceSubstring(p, i, "\t");
				style.RemoveNextElements(p+1, i-p);
				}
			}

		i = j;
		}
		while (text.LocateNextSubstring("\n", &i) && i < text.GetLength());

	// replace selection with cleaned text/style

	Paste(text, &style);
	SetSelection(r.first, r.first + text.GetLength() - 1);
}

/******************************************************************************
 CleanRightMargin

	If there is a selection, cleans up the right margin of each paragraph
	touched by the selection.  Otherwise, cleans up the right margin of the
	paragraph containing the caret and maintains the caret position.

	If coerce, paragraphs are detected by looking only for blank lines.
	Otherwise, they are detected by blank lines or a change in the line prefix.

	If anything is changed, returns kJTrue and sets *reformatRange to the
	range of reformatted text, excluding the last newline.

	The work done by this function can be changed by calling SetCRMRuleList()
	and overriding the virtual CRM*() functions.

 ******************************************************************************/

JBoolean
JTextEditor::CleanRightMargin
	(
	const JBoolean	coerce,
	JIndexRange*	reformatRange
	)
{
	if (TEIsDragging())
		{
		return kJFalse;
		}

	JBoolean changed = kJFalse;
	JIndexRange origTextRange;
	JString newText;
	JRunArray<JFont> newStyles;
	JIndex newCaretIndex;
	if (itsSelection.IsEmpty())
		{
		changed = PrivateCleanRightMargin(coerce, &origTextRange, &newText,
										  &newStyles, &newCaretIndex);
		}
	else
		{
		const JIndex endChar = GetParagraphEnd(itsSelection.last) - 1;

		JIndexRange range;
		JString text;
		JRunArray<JFont> styles;
		JIndex caretIndex;
		JBoolean first = kJTrue;
		while (1)
			{
			if (PrivateCleanRightMargin(coerce, &range, &text, &styles, &caretIndex))
				{
				origTextRange += range;
				newText       += text;
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												styles, 1, styles.GetElementCount());
				if (range.last < endChar)
					{
					(range.last)++;
					(origTextRange.last)++;
					newText.AppendCharacter('\n');
					newStyles.AppendElement(newStyles.GetLastElement());
					}
				}
			else
				{
				caretIndex = GetInsertionIndex();
				range.Set(GetParagraphStart(caretIndex),
						  GetParagraphEnd(caretIndex));

				assert( newText.IsEmpty() ||
						range.first == origTextRange.last + 1 );

				origTextRange += range;
				newText       += itsBuffer->GetSubstring(range);
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												*itsStyles, range.first, range.GetLength());
				}

			if (first)
				{
				newCaretIndex = caretIndex;
				first         = kJFalse;
				}

			if (range.last < endChar && IndexValid(range.last + 1))
				{
				SetCaretLocation(range.last + 1);
				}
			else
				{
				break;
				}
			}

		changed = kJTrue;
		}

	if (changed)
		{
		SetSelection(origTextRange, kJFalse);
		Paste(newText, &newStyles);
		SetCaretLocation(newCaretIndex);

		reformatRange->SetFirstAndLength(origTextRange.first, newText.GetLength());
		return kJTrue;
		}
	else
		{
		reformatRange->SetToNothing();
		return kJFalse;
		}
}

/******************************************************************************
 PrivateCleanRightMargin (private)

	*newText is the cleaned up version of the paragraph containing the caret
	or start of selection.

	*origTextRange contains the range that was cleaned up, excluding the last
	newline.

	*newCaretIndex is the index required to maintain the position of the caret.

	Returns kJFalse if the caret was not in a paragraph.

 ******************************************************************************/

JBoolean
JTextEditor::PrivateCleanRightMargin
	(
	const JBoolean		coerce,
	JIndexRange*		origTextRange,
	JString*			newText,
	JRunArray<JFont>*	newStyles,
	JIndex*				newCaretIndex
	)
	const
{
	origTextRange->SetToNothing();
	newText->Clear();
	newStyles->RemoveAll();
	*newCaretIndex = 0;

	if (itsBuffer->IsEmpty())
		{
		return kJFalse;
		}

	const JIndex caretChar = GetInsertionIndex();
	if (caretChar == itsBuffer->GetLength()+1 && EndsWithNewline())
		{
		return kJFalse;
		}

	JIndex charIndex, ruleIndex;
	JString firstLinePrefix, restLinePrefix;
	JSize firstPrefixLength, restPrefixLength;
	if (!CRMGetRange(caretChar, coerce, origTextRange, &charIndex,
					 &firstLinePrefix, &firstPrefixLength,
					 &restLinePrefix, &restPrefixLength, &ruleIndex))
		{
		return kJFalse;
		}

	if (caretChar <= charIndex)
		{
		*newCaretIndex = caretChar;
		}

	// read in each word, convert it, write it out

	JSize currentLineWidth = 0;
	JBoolean requireSpace  = kJFalse;

	JString wordBuffer, spaceBuffer;
	JRunArray<JFont> wordStyles;
	while (charIndex <= origTextRange->last)
		{
		JSize spaceCount;
		JIndex rnwCaretIndex = 0;
		const CRMStatus status =
			CRMReadNextWord(&charIndex, origTextRange->last,
							&spaceBuffer, &spaceCount, &wordBuffer, &wordStyles,
							currentLineWidth, caretChar, &rnwCaretIndex,
							*newText, requireSpace);
		requireSpace = kJTrue;

		if (status == kFinished)
			{
			assert( charIndex == origTextRange->last+1 );
			break;
			}
		else if (status == kFoundWord)
			{
			if (newText->IsEmpty())
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  firstLinePrefix, firstPrefixLength);
				}
			else
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  restLinePrefix, restPrefixLength);
				}

			if (rnwCaretIndex > 0)
				{
				*newCaretIndex = origTextRange->first + rnwCaretIndex - 1;
				}
			}
		else
			{
			assert( status == kFoundNewLine );

			CRMTossLinePrefix(&charIndex, origTextRange->last, ruleIndex);

			// CRMGetRange() ensures this is strictly *inside* the text,
			// so caretChar == charIndex will be caught elsewhere.

			if (*newCaretIndex == 0 && caretChar < charIndex)
				{
				*newCaretIndex = origTextRange->first + newText->GetLength();
				}
			}
		}

	if (caretChar == origTextRange->last+1)
		{
		*newCaretIndex = origTextRange->first + newText->GetLength();
		}

	assert( *newCaretIndex != 0 );
	assert( newText->GetLength() == newStyles->GetElementCount() );

	return kJTrue;
}

/*******************************************************************************
 CRMGetRange (private)

	Returns the range of characters to reformat.
	Returns kJFalse if the caret is not in a paragraph.

	caretChar is the current location of the caret.

	If coerce, we search forward and backward from the caret location
	for blank lines and include all the text between these blank lines.
	Blank lines are defined as lines that contain nothing but prefix characters.

	Otherwise, we first search backward until we find a blank line or a
	line prefix that can't generate the following one.  Then we search
	forward until we find a blank line or the prefix changes.

	The latter method is safer because it doesn't change the prefix of any of
	the lines.  The former is useful for forcing a change in the prefix.

 ******************************************************************************/

JBoolean
JTextEditor::CRMGetRange
	(
	const JIndex	caretChar,
	const JBoolean	coerce,
	JIndexRange*	range,
	JIndex*			textStartIndex,
	JString*		firstLinePrefix,
	JSize*			firstPrefixLength,
	JString*		restLinePrefix,
	JSize*			restPrefixLength,
	JIndex*			returnRuleIndex
	)
	const
{
	range->Set(GetParagraphStart(caretChar), GetParagraphEnd(caretChar));
	while (range->last > 0 && itsBuffer->GetCharacter(range->last) == '\n')
		{
		range->last--;
		}

	// If the line we are on is empty, quit immediately.

	JIndex tempStart = range->first;
	JString origLinePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (range->IsEmpty() ||
		!CRMGetPrefix(&tempStart, range->last,
					  &origLinePrefix, &prefixLength, &ruleIndex) ||
		CRMLineMatchesRest(*range))
		{
		return kJFalse;
		}

	// search backward for a blank line or a change in the prefix (if !coerce)
	// (If range->first==2, the line above us is blank.)

	JString currLinePrefix, nextLinePrefix = origLinePrefix;
	while (range->first > 2)
		{
		const JIndex newStart = GetParagraphStart(range->first - 1);
		tempStart             = newStart;
		ruleIndex             = 0;
		if (tempStart >= range->first - 1 ||
			!CRMGetPrefix(&tempStart, range->first - 2,
						  &currLinePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(newStart, range->first - 2)) ||
			(!coerce &&
			 CRMBuildRestPrefix(currLinePrefix, ruleIndex, &prefixLength) != nextLinePrefix))
			{
			break;
			}
		range->first   = newStart;
		nextLinePrefix = currLinePrefix;
		ruleIndex      = 0;
		}

	// search forward for a blank line or a change in the prefix (if !coerce)
	// (If range->last==bufLength-1, the text ends with a newline.)

	*textStartIndex  = range->first;
	*returnRuleIndex = 0;
	const JBoolean hasText =
		CRMGetPrefix(textStartIndex, range->last,
					 firstLinePrefix, firstPrefixLength, returnRuleIndex);
	assert( hasText );

	*restLinePrefix = CRMBuildRestPrefix(*firstLinePrefix, *returnRuleIndex,
										 restPrefixLength);

	while (range->last < itsBuffer->GetLength()-1)
		{
		tempStart     = range->last + 2;
		JIndex newEnd = GetParagraphEnd(tempStart);
		if (itsBuffer->GetCharacter(newEnd) == '\n')	// could hit end of text instead
			{
			newEnd--;
			}

		JIndex tempRuleIndex = *returnRuleIndex;
		if (newEnd < tempStart ||
			!CRMGetPrefix(&tempStart, newEnd,
						  &currLinePrefix, &prefixLength, &tempRuleIndex) ||
			(!coerce && currLinePrefix != *restLinePrefix))
			{
			break;
			}
		range->last = newEnd;
		}

	return kJTrue;
}

/*******************************************************************************
 CRMGetPrefix (private)

	Returns the prefix to be used for each line and updates *startChar to point
	to the first character after the prefix.

	*prefixLength is set to the length of *linePrefix in characters.  This
	can be greater than linePrefix->GetLength() because of tabs.

	Returns kJFalse if the entire range qualifies as a prefix.

 ******************************************************************************/

JBoolean
JTextEditor::CRMGetPrefix
	(
	JIndex*			startChar,
	const JIndex	endChar,
	JString*		linePrefix,
	JSize*			prefixLength,
	JIndex*			ruleIndex
	)
	const
{
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*startChar, endChar), ruleIndex);

	*startChar    = prefixRange.last + 1;
	*linePrefix   = itsBuffer->GetSubstring(prefixRange);
	*prefixLength = CRMCalcPrefixLength(linePrefix);
	return JConvertToBoolean(*startChar <= endChar);
}

/*******************************************************************************
 CRMMatchPrefix (private)

	Returns the range of characters that qualifies as a prefix.

	To match a prefix, all CRMRule::first's and [ \t]* are tried.
	The longest match is used.

	If *ruleIndex > 0, CRMRule::rest for the specified rule is tried *first*,
	and if this matches, it preempts everything else.

	When the function returns, *ruleIndex is the CRMRule::first that matched.
	(It remains 0 if the default rule is used, since this doesn't have "rest".)

 ******************************************************************************/

static const JRegex defaultCRMPrefixRegex = "[ \t]*";

JIndexRange
JTextEditor::CRMMatchPrefix
	(
	const JIndexRange&	textRange,
	JIndex*				ruleIndex
	)
	const
{
	JIndexRange matchRange;

	if (itsCRMRuleList != NULL && *ruleIndex > 0)
		{
		const CRMRule rule = itsCRMRuleList->GetElement(*ruleIndex);
		if ((rule.rest)->MatchWithin(*itsBuffer, textRange, &matchRange) &&
			matchRange.first == textRange.first)
			{
			return matchRange;
			}
		}

	JIndexRange range;
	if (itsCRMRuleList != NULL)
		{
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.first)->MatchWithin(*itsBuffer, textRange, &range) &&
				range.first == textRange.first &&
				range.last  >  matchRange.last)
				{
				matchRange = range;
				*ruleIndex = i;
				}
			}
		}

	// check equality of range::last in case prefix is empty

	const JBoolean defMatch =
		defaultCRMPrefixRegex.MatchWithin(*itsBuffer, textRange, &range);
	assert( defMatch && range.first == textRange.first );
	if (range.last >= matchRange.last || itsCRMRuleList == NULL)
		{
		matchRange = range;
		*ruleIndex = 0;
		}

	return matchRange;
}

/*******************************************************************************
 CRMLineMatchesRest (private)

	Returns kJTrue if the given range is matched by any "rest" pattern.
	Used at beginning of CRMGetRange as part of check if line is empty.

 ******************************************************************************/

JBoolean
JTextEditor::CRMLineMatchesRest
	(
	const JIndexRange& range
	)
	const
{
	if (itsCRMRuleList != NULL)
		{
		JIndexRange matchRange;
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.rest)->MatchWithin(*itsBuffer, range, &matchRange) &&
				range == matchRange)
				{
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/*******************************************************************************
 CRMCalcPrefixLength (private)

	Returns the length of *linePrefix in characters.  This can be greater
	than linePrefix->GetLength() because of tabs.  *linePrefix may be modified.

 ******************************************************************************/

JSize
JTextEditor::CRMCalcPrefixLength
	(
	JString* linePrefix
	)
	const
{
	if (linePrefix->IsEmpty())
		{
		return 0;
		}

	const JString s    = *linePrefix;
	JSize prefixLength = 0;
	linePrefix->Clear();

	const JSize length = s.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = s.GetCharacter(i);
		if (c == '\t')
			{
			CRMConvertTab(linePrefix, &prefixLength, 0);
			}
		else
			{
			linePrefix->AppendCharacter(c);
			prefixLength++;
			}
		}

	return prefixLength;
}

/*******************************************************************************
 CRMBuildRestPrefix (private)

	Returns the line prefix to be used for all lines after the first one.

 ******************************************************************************/

JString
JTextEditor::CRMBuildRestPrefix
	(
	const JString&	firstLinePrefix,
	const JIndex	ruleIndex,
	JSize*			prefixLength
	)
	const
{
	JString s = firstLinePrefix;
	if (itsCRMRuleList != NULL && ruleIndex > 0)
		{
		JArray<JIndexRange> matchList;
		const CRMRule rule   = itsCRMRuleList->GetElement(ruleIndex);
		const JBoolean match = (rule.first)->Match(s, &matchList);
		assert( match &&
				(matchList.GetFirstElement()).first == 1 &&
				(matchList.GetFirstElement()).last  == s.GetLength() );

		JIndexRange newRange;
		(rule.first)->Replace(&s, matchList, &newRange);
		}

	*prefixLength = CRMCalcPrefixLength(&s);
	return s;
}

/*******************************************************************************
 CRMTossLinePrefix (private)

	Increments *charIndex past whatever qualifies as a line prefix.

	The default is to toss all character approved by CRMIsPrefixChar().

 ******************************************************************************/

void
JTextEditor::CRMTossLinePrefix
	(
	JIndex*			charIndex,
	const JIndex	endChar,
	const JIndex	origRuleIndex
	)
	const
{
	JIndex ruleIndex = origRuleIndex;
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*charIndex, endChar), &ruleIndex);
	*charIndex = prefixRange.last + 1;
}

/*******************************************************************************
 CRMConvertTab (virtual protected)

	Appends the tab to *charBuffer and increments *charCount appropriately.
	The default is to physically append the tab character and use
	CRMGetTabWidth() to increment *charCount.  Note that *charCount may be
	longer than charBuffer->GetLength().

	currentLineWidth is the number of characters on the line -excluding-
	what is in *charBuffer.

	Derived classes can override this to, for example, append some spaces
	instead of a tab character.  (How's that for splitting an infinitive?)

 ******************************************************************************/

void
JTextEditor::CRMConvertTab
	(
	JString*	charBuffer,
	JSize*		charCount,
	const JSize	currentLineWidth
	)
	const
{
	charBuffer->AppendCharacter('\t');
	*charCount += CRMGetTabWidth(currentLineWidth + *charCount);
}

/*******************************************************************************
 CRMGetTabWidth (virtual protected)

	Returns the number of spaces to which the tab is equivalent.
	The default is to round up to the nearest multiple of GetCRMTabCharCount().
	The default value for this is 8 since this is what all UNIX programs use.

	textColumn starts at zero at the left margin.

 ******************************************************************************/

JSize
JTextEditor::CRMGetTabWidth
	(
	const JIndex textColumn
	)
	const
{
	return itsCRMTabCharCount - (textColumn % itsCRMTabCharCount);
}

/*******************************************************************************
 CRMIsEOS (private)

 ******************************************************************************/

inline int
JTextEditor::CRMIsEOS
	(
	const JCharacter c
	)
	const
{
	return (c == '.' || c == '?' || c == '!');
}

/*******************************************************************************
 CRMReadNextWord (private)

	Read one block of { spaces + word (non-spaces) } from itsBuffer, starting
	at *charIndex, stopping if we get as far as endIndex.  *spaceBuffer
	contains the whitespace (spaces + tabs) that was found.  *spaceCount is
	the equivalent number of spaces.  *wordBuffer contains the word.
	*charIndex is incremented to point to the next character to read.

	Newlines are treated separately.  If a newline is encountered while reading
	spaces, we throw out the spaces and return kFoundNewLine.  If a newline
	is encountered while reading a word, we leave it for the next time, when
	we immediately return kFoundNewLine.

	When we pass the position origCaretIndex, we set *newCaretIndex to be
	the index into *spaceBuffer+*wordBuffer.  Otherwise, we do not change
	*newCaretIndex.

 ******************************************************************************/

JTextEditor::CRMStatus
JTextEditor::CRMReadNextWord
	(
	JIndex*				charIndex,
	const JIndex		endIndex,
	JString*			spaceBuffer,
	JSize*				spaceCount,
	JString*			wordBuffer,
	JRunArray<JFont>*	wordStyles,
	const JSize			currentLineWidth,
	const JIndex		origCaretIndex,
	JIndex*				newCaretIndex,
	const JString&		newText,
	const JBoolean		requireSpace
	)
	const
{
	// read the whitespace

	spaceBuffer->Clear();
	*spaceCount = 0;

	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + 1;
			}

		const JCharacter c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ')
			{
			(*charIndex)++;
			spaceBuffer->AppendCharacter(c);
			(*spaceCount)++;
			}
		else if (c == '\t')
			{
			(*charIndex)++;
			CRMConvertTab(spaceBuffer, spaceCount, currentLineWidth);
			}
		else if (c == '\n')			// we can ignore the spaces
			{
			(*charIndex)++;
			spaceBuffer->Clear();
			*spaceCount = 0;
			return kFoundNewLine;
			}
		else						// found beginning of word
			{
			break;
			}
		}

	if (*charIndex == endIndex+1)	// we can ignore the spaces
		{
		return kFinished;
		}

	// read the word

	wordBuffer->Clear();

	const JIndex wordStart = *charIndex;
	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + wordBuffer->GetLength() + 1;
			}

		const JCharacter c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ' || c == '\t' || c == '\n')
			{
			break;
			}

		wordBuffer->AppendCharacter(c);
		(*charIndex)++;
		}

	wordStyles->RemoveAll();
	wordStyles->InsertElementsAtIndex(1, *itsStyles, wordStart, wordBuffer->GetLength());

	// After a newline, the whitespace may have been tossed
	// as belonging to the prefix, but we still need some space.
	//
	// Punctuation is assumed to be the end of a sentence if the
	// following word does not start with a lower case letter.

	if (*spaceCount == 0 && requireSpace)
		{
		JCharacter c1 = newText.GetLastCharacter();
		JCharacter c2 = '\0';
		if (newText.GetLength() > 1)
			{
			c2 = newText.GetCharacter(newText.GetLength()-1);
			}
		if ((CRMIsEOS(c1) || (CRMIsEOS(c2) && c1 =='\"')) &&
			!JIsLower(wordBuffer->GetFirstCharacter()))
			{
			*spaceBuffer = "  ";
			}
		else
			{
			*spaceBuffer = " ";
			}

		*spaceCount = spaceBuffer->GetLength();
		if (*newCaretIndex > 0)
			{
			*newCaretIndex += *spaceCount;
			}
		}

	return kFoundWord;
}

/*******************************************************************************
 CRMAppendWord (private)

	Add the spaces and word to new text, maintaining the required line width.

	If *newCaretIndex>0, we convert it from an index in spaceBuffer+wordBuffer
	to an index in newText.

 ******************************************************************************/

void
JTextEditor::CRMAppendWord
	(
	JString*				newText,
	JRunArray<JFont>*		newStyles,
	JSize*					currentLineWidth,
	JIndex*					newCaretIndex,
	const JString&			spaceBuffer,
	const JSize				spaceCount,
	const JString&			wordBuffer,
	const JRunArray<JFont>&	wordStyles,
	const JString&			linePrefix,
	const JSize				prefixLength
	)
	const
{
	const JSize newLineWidth = *currentLineWidth + spaceCount + wordBuffer.GetLength();
	if (*currentLineWidth == 0 || newLineWidth > itsCRMLineWidth)
		{
		// calculate prefix font

		JFont prefixFont = wordStyles.GetFirstElement();
		prefixFont.ClearStyle();

		// terminate previous line

		if (!newText->IsEmpty())
			{
			newText->AppendCharacter('\n');
			newStyles->AppendElement(prefixFont);
			}
		if (!linePrefix.IsEmpty())
			{
			*newText += linePrefix;
			newStyles->AppendElements(prefixFont, linePrefix.GetLength());
			}

		// write word

		if (*newCaretIndex > 0)
			{
			if (*newCaretIndex <= spaceCount)
				{
				*newCaretIndex = 1;				// in spaces that we toss
				}
			else
				{
				*newCaretIndex -= spaceCount;	// in word
				}

			*newCaretIndex += newText->GetLength();
			}

		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());
		*currentLineWidth = prefixLength + wordBuffer.GetLength();
		}
	else	// newLineWidth <= itsCRMLineWidth
		{
		// append spaces + word at end of line

		if (*newCaretIndex > 0)
			{
			*newCaretIndex += newText->GetLength();
			}

		*newText += spaceBuffer;
		newStyles->AppendElements(JCalcWSFont(newStyles->GetLastElement(),
											  wordStyles.GetFirstElement()),
								  spaceBuffer.GetLength());
		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());

		if (newLineWidth < itsCRMLineWidth)
			{
			*currentLineWidth = newLineWidth;
			}
		else	// newLineWidth == itsCRMLineWidth
			{
			*currentLineWidth = 0;
			}
		}
}

/******************************************************************************
 SetCRMRuleList

	To find the first line of a paragraph, one searches backward for a blank
	line:  one that matches any CRMRule::first.  The following line is the
	beginning of the paragraph.

	Each following line is part of the paragraph if it is not blank:
	no CRMRule::first matches the entire line, and neither does the
	CRMRule::rest corresponding to the CRMRule::first that matched the first
	line of the paragraph.

	The prefix of the first line is unchanged.  The prefix of the following
	lines is calculated from calling (CRMRule::first)->Replace() on the
	prefix of the first line.

 ******************************************************************************/

void
JTextEditor::SetCRMRuleList
	(
	CRMRuleList*	ruleList,
	const JBoolean	teOwnsRuleList
	)
{
	ClearCRMRuleList();

	itsCRMRuleList      = ruleList;
	itsOwnsCRMRulesFlag = teOwnsRuleList;
}

/******************************************************************************
 ClearCRMRuleList

	With no other rules, the default is to match [ \t]*

 ******************************************************************************/

void
JTextEditor::ClearCRMRuleList()
{
	if (itsOwnsCRMRulesFlag && itsCRMRuleList != NULL)
		{
		itsCRMRuleList->DeleteAll();
		jdelete itsCRMRuleList;
		}

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
}

/******************************************************************************
 CRMRule functions

 ******************************************************************************/

JRegex*
JTextEditor::CRMRule::CreateFirst
	(
	const JCharacter* pattern,
	const JCharacter* replacePattern
	)
{
	JRegex* r = jnew JRegex(pattern);
	assert( r != NULL );
	const JError err = r->SetReplacePattern(replacePattern);
	assert_ok( err );
	return r;
}

JRegex*
JTextEditor::CRMRule::CreateRest
	(
	const JCharacter* pattern
	)
{
	JRegex* r = jnew JRegex(pattern);
	assert( r != NULL );
	return r;
}

/******************************************************************************
 CRMRuleList functions

 ******************************************************************************/

JTextEditor::CRMRuleList::CRMRuleList
	(
	const CRMRuleList& source
	)
	:
	JArray<CRMRule>()
{
	const JSize count = source.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CRMRule r = source.GetElement(i);
		AppendElement(
			CRMRule((r.first)->GetPattern(), (r.rest)->GetPattern(),
					(r.first)->GetReplacePattern()));
		}
}

void
JTextEditor::CRMRuleList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CRMRule r = GetElement(i);
		jdelete r.first;
		jdelete r.rest;
		}

	RemoveAll();
}

/******************************************************************************
 SetCRMLineWidth

 ******************************************************************************/

void
JTextEditor::SetCRMLineWidth
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMLineWidth = charCount;
}

/******************************************************************************
 SetCRMTabCharCount

 ******************************************************************************/

void
JTextEditor::SetCRMTabCharCount
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMTabCharCount = charCount;
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JTextEditor::Undo()
{
	assert( itsUndoState == kIdle );

	if (TEIsDragging())
		{
		return;
		}

	JTEUndoBase* undo;
	const JBoolean hasUndo = GetCurrentUndo(&undo);
	if (hasUndo && itsType == kFullEditor)
		{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
	else if (hasUndo)
		{
		ClearUndo();
		}
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
JTextEditor::Redo()
{
	assert( itsUndoState == kIdle );

	if (TEIsDragging())
		{
		return;
		}

	JTEUndoBase* undo;
	const JBoolean hasUndo = GetCurrentRedo(&undo);
	if (hasUndo && itsType == kFullEditor)
		{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
	else if (hasUndo)
		{
		ClearUndo();
		}
}

/******************************************************************************
 DeactivateCurrentUndo

 ******************************************************************************/

void
JTextEditor::DeactivateCurrentUndo()
{
	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo))
		{
		undo->Deactivate();
		}
}

/******************************************************************************
 ClearUndo

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
JTextEditor::ClearUndo()
{
	jdelete itsUndo;
	itsUndo = NULL;

	if (itsUndoList != NULL)
		{
		itsUndoList->CleanOut();
		}
	itsFirstRedoIndex = 1;
	ClearLastSaveLocation();
}

/******************************************************************************
 UseMultipleUndo

	You probably never need to turn off multiple undo unless you
	are running out of memory.

	If you call this in your constructor, you should call
	SetLastSaveLocation() afterwards.

 ******************************************************************************/

void
JTextEditor::UseMultipleUndo
	(
	const JBoolean useMultiple
	)
{
	if (useMultiple && itsUndoList == NULL)
		{
		ClearUndo();

		itsUndoList = jnew JPtrArray<JTEUndoBase>(JPtrArrayT::kDeleteAll,
												 itsMaxUndoCount+1);
		assert( itsUndoList != NULL );
		}
	else if (!useMultiple && itsUndoList != NULL)
		{
		ClearUndo();

		jdelete itsUndoList;
		itsUndoList = NULL;
		}
}

/******************************************************************************
 SetUndoDepth

 ******************************************************************************/

void
JTextEditor::SetUndoDepth
	(
	const JSize maxUndoCount
	)
{
	assert( maxUndoCount > 0 );

	itsMaxUndoCount = maxUndoCount;
	ClearOutdatedUndo();
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

JBoolean
JTextEditor::GetCurrentUndo
	(
	JTEUndoBase** undo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex > 1)
		{
		*undo = itsUndoList->GetElement(itsFirstRedoIndex - 1);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*undo = itsUndo;
		return JConvertToBoolean( *undo != NULL );
		}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

JBoolean
JTextEditor::GetCurrentRedo
	(
	JTEUndoBase** redo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex <= itsUndoList->GetElementCount())
		{
		*redo = itsUndoList->GetElement(itsFirstRedoIndex);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*redo = itsUndo;
		return JConvertToBoolean( *redo != NULL );
		}
}

/******************************************************************************
 NewUndo (private)

	Register a jnew Undo object.

	itsFirstRedoIndex points to the first redo object in itsUndoList.
	1 <= itsFirstRedoIndex <= itsUndoList->GetElementCount()+1
	Minimum => everything is redo
	Maximum => everything is undo

 ******************************************************************************/

void
JTextEditor::NewUndo
	(
	JTEUndoBase*	undo,
	const JBoolean	isNew
	)
{
	if (!isNew)
		{
		if (itsBcastAllTextChangedFlag)
			{
			Broadcast(TextChanged());
			}
		return;
		}

	if (itsUndoList != NULL && itsUndoState == kIdle)
		{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
			{
			itsUndoList->DeleteElement(i);
			}

		if (itsLastSaveRedoIndex > 0 &&
			itsFirstRedoIndex < JIndex(itsLastSaveRedoIndex))
			{
			ClearLastSaveLocation();
			}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		ClearOutdatedUndo();
		assert( !itsUndoList->IsEmpty() );
		}

	else if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(kJTrue);
		undo->Deactivate();
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		undo->Deactivate();
		}

	else
		{
		jdelete itsUndo;
		itsUndo = undo;
		}

	Broadcast(TextChanged());
}

/******************************************************************************
 SameUndo (private)

	Called by Undo objects to notify us that there was actually nothing
	to do, so the same object can be re-used.

 ******************************************************************************/

void
JTextEditor::SameUndo
	(
	JTEUndoBase* undo
	)
{
	assert( itsUndoState != kIdle );

	if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		assert( undo == itsUndoList->GetElement(itsFirstRedoIndex) );

		undo->SetRedo(kJTrue);
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		assert( undo == itsUndoList->GetElement(itsFirstRedoIndex) );
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		}

	else if (itsUndoList == NULL)
		{
		assert( undo == itsUndo );
		}

	undo->Deactivate();

	// nothing changed, so we don't broadcast TextChanged
}

/******************************************************************************
 ReplaceUndo (private)

 ******************************************************************************/

void
JTextEditor::ReplaceUndo
	(
	JTEUndoBase* oldUndo,
	JTEUndoBase* newUndo
	)
{
#ifndef NDEBUG

	assert( itsUndoState != kIdle );

	if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex - 1) );
		}
	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex) );
		}
	else
		{
		assert( oldUndo == itsUndo );
		}

#endif

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 ClearOutdatedUndo (private)

 ******************************************************************************/

void
JTextEditor::ClearOutdatedUndo()
{
	if (itsUndoList != NULL)
		{
		JSize undoCount = itsUndoList->GetElementCount();
		while (undoCount > itsMaxUndoCount)
			{
			itsUndoList->DeleteElement(1);
			undoCount--;
			itsFirstRedoIndex--;
			itsLastSaveRedoIndex--;

			// If we have to throw out redo's, we have to toss everything.

			if (itsFirstRedoIndex == 0)
				{
				ClearUndo();
				break;
				}
			}
		}
}

/******************************************************************************
 GetTypingUndo (private)

	Return the active JTEUndoTyping object.  If the current undo object is
	not an active JTEUndoTyping object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTyping*
JTextEditor::GetTypingUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoTyping* typingUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(typingUndo = dynamic_cast<JTEUndoTyping*>(undo)) != NULL &&
		typingUndo->IsActive())
		{
		assert( itsSelection.IsEmpty() );
		*isNew = kJFalse;
		return typingUndo;
		}
	else
		{
		typingUndo = jnew JTEUndoTyping(this);
		assert( typingUndo != NULL );

		*isNew = kJTrue;
		return typingUndo;
		}
}

/******************************************************************************
 GetStyleUndo (private)

	Return the active JTEUndoStyle object.  If the current undo object is
	not an active JTEUndoStyle object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoStyle*
JTextEditor::GetStyleUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoStyle* styleUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(styleUndo = dynamic_cast<JTEUndoStyle*>(undo)) != NULL &&
		styleUndo->IsActive())
		{
		assert( !itsSelection.IsEmpty() );
		*isNew = kJFalse;
		return styleUndo;
		}
	else
		{
		styleUndo = jnew JTEUndoStyle(this);
		assert( styleUndo != NULL );

		*isNew = kJTrue;
		return styleUndo;
		}
}

/******************************************************************************
 GetTabShiftUndo (private)

	Return the active JTEUndoTabShift object.  If the current undo object is
	not an active JTEUndoTabShift object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTabShift*
JTextEditor::GetTabShiftUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoTabShift* tabShiftUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(tabShiftUndo = dynamic_cast<JTEUndoTabShift*>(undo)) != NULL &&
		tabShiftUndo->IsActive())
		{
		*isNew = kJFalse;
		return tabShiftUndo;
		}
	else
		{
		tabShiftUndo = jnew JTEUndoTabShift(this);
		assert( tabShiftUndo != NULL );

		*isNew = kJTrue;
		return tabShiftUndo;
		}
}

/******************************************************************************
 InsertText (private)

	*** Caller must call Recalc().  Nothing can be selected.

	Returns number of characters / bytes that were actually inserted.

	style can be NULL.

	In second version, iterator must be positioned at insertion index.

 ******************************************************************************/

JTextEditor::TextCount
JTextEditor::InsertText
	(
	const TextIndex&		index,
	const JString&			text,
	const JRunArray<JFont>*	style	// can be NULL
	)
{
	assert( itsSelection.IsEmpty() );

	JStringIterator iter(&itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	return InsertText(&iter, itsStyles, text, style, &itsInsertionFont);
}

JTextEditor::TextCount
JTextEditor::InsertText
	(
	JStringIterator*		targetText,
	JRunArray<JFont>*		targetStyle,
	const JString&			text,
	const JRunArray<JFont>*	style,			// can be NULL
	const JFont*			defaultFont		// can be NULL
	)
{
	if (text.IsEmpty())
		{
		return TextCount();
		}

	JString* cleanText           = NULL;
	JRunArray<JFont>* cleanStyle = NULL;

	JBoolean okToInsert;
	const JBoolean allocated =
		CleanText(text, style, &cleanText, &cleanStyle, &okToInsert);

	if (!okToInsert)
		{
		return TextCount();
		}

	if (!allocated)
		{
		cleanText = const_cast<JString*>(&text);
		}

	// insert the text

	const JIndex charIndex = targetText->GetNextCharacterIndex();
	if (cleanStyle != NULL)
		{
		targetStyle->InsertElementsAtIndex(charIndex, *cleanStyle, 1, cleanStyle->GetElementCount());
		}
	else
		{
		targetStyle->InsertElementsAtIndex(
			charIndex,
			defaultFont != NULL ? *defaultFont : CalcInsertionFont(*targetText, *targetStyle),
			cleanText->GetCharacterCount());
		}

	// modify targetText only after calling CalcInsertionFont()

	targetText->Insert(*cleanText);

	const TextCount result(cleanText->GetCharacterCount(), cleanText->GetByteCount());

	if (allocated)
		{
		jdelete cleanText;
		}

	return result;
}

/******************************************************************************
 CleanText (private)

	Removes illegal characters, converts to UNIX newline format, lets
	derived class perform additional filtering.  Returns true if the text
	was modified.

	*okToInsert is true if derived class filtering accepted the text.

	style can be NULL.

 ******************************************************************************/

#define COPY_FOR_CLEAN_TEXT \
	if (*cleanText == NULL) \
		{ \
		*cleanText = jnew JString(text); \
		assert( *cleanText != NULL ); \
		if (style != NULL) \
			{ \
			*cleanStyle = jnew JRunArray<JFont>(*style); \
			assert( *cleanStyle != NULL ); \
			} \
		}

JBoolean
JTextEditor::CleanText
	(
	const JString&			text,
	const JRunArray<JFont>*	style,	// can be NULL
	JString**				cleanText,
	JRunArray<JFont>**		cleanStyle,
	JBoolean*				okToInsert
	)
{
	if (text.IsEmpty())
		{
		return kJFalse;
		}

	assert( style == NULL || text.GetCharacterCount() == style->GetElementCount() );

	*cleanText  = NULL;
	*cleanStyle = NULL;

	// remove illegal characters

	if (ContainsIllegalChars(text))
		{
		COPY_FOR_CLEAN_TEXT

		RemoveIllegalChars(cleanText, cleanStyle);
		}

	// convert from DOS format -- deleting is n^2, so we copy instead
	// (not using Split because the text could be very large)

	if ((*cleanText != NULL && **cleanText.Contains(kDOSNewline)) ||
		(*cleanText == NULL && text.Contains(kDOSNewline)))
		{
		COPY_FOR_CLEAN_TEXT

		JString tmpText;
		tmpText.SetBlockSize(cleanText->GetByteCount()+256);

		JRunArray<JFont> tmpStyle;
		if (*cleanStyle != NULL)
			{
			tmpStyle.SetBlockSize(**cleanStyle.GetRunCount()+16);
			}

		JStringIterator iter(cleanText);
		JUtf8Character c;

		JBoolean done = kJFalse;
		iter.BeginMatch();
		while (!done)
			{
			done = !iter.Next("\r");	// ensures that we append trailing text

			const JStringMatch& m = iter.FinishMatch();
			if (!m.IsEmpty())
				{
				tmpText.Append(m.GetString());
				if (*cleanStyle != NULL)
					{
					tmpStyle.AppendSlice(**cleanStyle, m.GetCharacterRange());
					}
				}
			iter.BeginMatch();
			}

		**cleanText = tmpText;
		if (*cleanStyle != NULL)
			{
			**cleanStyle = tmpStyle;
			}
		}

	// convert from Macintosh format

	else if ((*cleanText != NULL && cleanText->Contains(kMacintoshNewline)) ||
			 (*cleanText == NULL && text.Contains(kMacintoshNewline)))
		{
		COPY_FOR_CLEAN_TEXT

		ConvertFromMacintoshNewlinetoUNIXNewline(*cleanText);
		}

	// allow derived classes to make additional changes
	// (last so we don't pass anything illegal to FilterText())

	*okToInsert = kJTrue;
	if (NeedsToFilterText(*cleanText != NULL ? **cleanText : text))
		{
		COPY_FOR_CLEAN_TEXT

		*okToInsert = FilterText(*cleanText, *cleanStyle);
		}

	return JI2B( *cleanText != NULL );
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JTextEditor::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JTextEditor::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	return kJTrue;
}

/******************************************************************************
 DeleteText (private)

	*** Caller must call Recalc().

 ******************************************************************************/

void
JTextEditor::DeleteText
	(
	const JCharacterRange&	charRange,
	const JUtf8ByteRange&	byteRange
	)
{
	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, charRange.first, byteRange.first);
	DeleteText(&iter, charRange.GetLength());
}

void
JTextEditor::DeleteText
	(
	JStringIterator*	iter,
	const JSize			charCount
	)
{
	itsStyles->RemoveNextElements(iter->GetNextCharacterIndex(), charCount);
	iter->RemoveNext(charCount);
}

/******************************************************************************
 Paginate (protected)

	Returns breakpoints for cutting text into pages.  The first breakpoint
	is always zero, and the last breakpoint is the height of the text.
	Thus, it is easy to calculate the width of what is printed on each page
	from (breakpt->GetElement(i+1) - breakpt->GetElement(i) + 1).

 ******************************************************************************/

void
JTextEditor::Paginate
	(
	const JCoordinate		pageHeight,
	JArray<JCoordinate>*	breakpts
	)
	const
{
	assert( pageHeight > 0 );

	breakpts->RemoveAll();
	breakpts->AppendElement(0);

	const JSize count = GetLineCount();

	JRunArrayIterator<LineGeometry> iter(itsLineGeom);
	LineGeometry geom;

	JIndex prev = 1, i = 0;
	JCoordinate h = 0;
	do
		{
		// find the number of strips that will fit on this page

		while (i < count && h <= pageHeight)
			{
			i++;
			const JBoolean ok = iter.Next(&geom);
			assert( ok );
			h += geom.height;
			}

		JCoordinate pageH = h;
		if (h > pageHeight && i > prev)
			{
			// The last line didn't fit on the page,
			// so leave it for the next page.

			pageH -= geom.height;
			i--;
			const JBoolean ok = iter.Prev(&geom);
			assert( ok );
			h = 0;
			}
		else if (h > pageHeight)
			{
			// The line won't fit on any page.  Put
			// as much as possible on this page and leave
			// the rest for the next page.

			pageH = pageHeight;
			h    -= pageHeight;
			}
		else
			{
			// Everything fits on the page, so there is no residual.

			h = 0;
			}

		breakpts->AppendElement(breakpts->GetLastElement() + pageH);
		prev = i;
		}
		while (i < count || h > 0);
}

/******************************************************************************
 Print

	This function could be optimized to save the necessary state so
	TESetBoundsWidth() doesn't have to be called again at the end.

 ******************************************************************************/

void
JTextEditor::Print
	(
	JPagePrinter& p
	)
{
	// avoid notifing GUI from within Recalc()
	// (We put it here to allow OpenDocument() to adjust things.)

	itsIsPrintingFlag = kJTrue;

	if (!p.OpenDocument())
		{
		itsIsPrintingFlag = kJFalse;
		return;
		}

	const JCoordinate headerHeight = GetPrintHeaderHeight(p);
	const JCoordinate footerHeight = GetPrintFooterHeight(p);

	// We deactivate before printing so the selection or the
	// caret won't be printed.

	const JBoolean savedActive = TEIsActive();
	TEDeactivate();

	// adjust to the width of the page

	const JCoordinate savedWidth = TEGetBoundsWidth();
	const JCoordinate pageWidth  = p.GetPageWidth();
	TESetBoundsWidth(pageWidth);

	// paginate

	JArray<JCoordinate> breakpts;
	Paginate(p.GetPageHeight()-headerHeight-footerHeight, &breakpts);

	// print each page

	const JSize pageCount = breakpts.GetElementCount() - 1;

	JBoolean cancelled = kJFalse;
	for (JIndex i=1; i<=pageCount; i++)
		{
		if (!p.NewPage())
			{
			cancelled = kJTrue;
			break;
			}

		if (headerHeight > 0)
			{
			DrawPrintHeader(p, headerHeight);
			p.LockHeader(headerHeight);
			}
		if (footerHeight > 0)
			{
			DrawPrintFooter(p, footerHeight);
			p.LockFooter(footerHeight);
			}

		JPoint topLeft(0, breakpts.GetElement(i));
		JPoint bottomRight(pageWidth, breakpts.GetElement(i+1));
		JRect clipRect(JPoint(0,0), bottomRight - topLeft);
		p.SetClipRect(clipRect);

		p.ShiftOrigin(-topLeft);
		TEDraw(p, JRect(topLeft,bottomRight));
		}

	if (!cancelled)
		{
		p.CloseDocument();
		}

	itsIsPrintingFlag = kJFalse;

	// restore the original state and width

	if (savedActive)
		{
		TEActivate();
		}

	TESetBoundsWidth(savedWidth);
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.

 ******************************************************************************/

JCoordinate
JTextEditor::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

JCoordinate
JTextEditor::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

void
JTextEditor::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
}

void
JTextEditor::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
}

/******************************************************************************
 GetCmdStatus (protected)

	Returns an array indicating which commands can be performed in the
	current state.

 ******************************************************************************/

JArray<JBoolean>
JTextEditor::GetCmdStatus
	(
	JString*	crmActionText,
	JString*	crm2ActionText,
	JBoolean*	isReadOnly
	)
	const
{
	JArray<JBoolean> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
		{
		flags.AppendElement(kJFalse);
		}

	*isReadOnly = kJTrue;

	if (itsType == kStaticText || !itsActiveFlag)
		{
		return flags;
		}

	// Edit menu

	if (itsSelection.IsEmpty())
		{
		*crmActionText  = JGetString("CRMCaretAction::JTextEditor");
		*crm2ActionText = JGetString("CRM2CaretAction::JTextEditor");
		}
	else
		{
		flags.SetElement(kCopyCmd, kJTrue);
		*crmActionText  = JGetString("CRMSelectionAction::JTextEditor");
		*crm2ActionText = JGetString("CRM2SelectionAction::JTextEditor");
		}

	flags.SetElement(kSelectAllCmd,      kJTrue);
	flags.SetElement(kToggleReadOnlyCmd, kJTrue);
	flags.SetElement(kShowWhitespaceCmd, kJTrue);

	if (itsType == kFullEditor)
		{
		*isReadOnly = kJFalse;

		flags.SetElement(kPasteCmd, kJTrue);
		if (!itsSelection.IsEmpty())
			{
			flags.SetElement(kCutCmd,       kJTrue);
			flags.SetElement(kDeleteSelCmd, kJTrue);
			}

		if (!itsBuffer->IsEmpty())
			{
			flags.SetElement(kCheckSpellingCmd,      kJTrue);
			flags.SetElement(kCleanRightMarginCmd,   kJTrue);
			flags.SetElement(kCoerceRightMarginCmd,  kJTrue);
			flags.SetElement(kShiftSelLeftCmd,       kJTrue);
			flags.SetElement(kShiftSelRightCmd,      kJTrue);
			flags.SetElement(kForceShiftSelLeftCmd,  kJTrue);
			flags.SetElement(kCleanAllWhitespaceCmd, kJTrue);
			flags.SetElement(kCleanAllWSAlignCmd,    kJTrue);

			if (!itsSelection.IsEmpty())
				{
				flags.SetElement(kCheckSpellingSelCmd,   kJTrue);
				flags.SetElement(kCleanWhitespaceSelCmd, kJTrue);
				flags.SetElement(kCleanWSAlignSelCmd,    kJTrue);
				}
			}

		if (itsUndoList != NULL)
			{
			flags.SetElement(kUndoCmd, JConvertToBoolean( itsFirstRedoIndex > 1 ));
			flags.SetElement(kRedoCmd,
				JConvertToBoolean( itsFirstRedoIndex <= itsUndoList->GetElementCount() ));
			}
		else if (itsUndo != NULL)
			{
			const JBoolean isRedo = itsUndo->IsRedo();
			flags.SetElement(kUndoCmd, !isRedo);
			flags.SetElement(kRedoCmd, isRedo);
			}
		}

	// Search & Replace menu

	flags.SetElement(kFindDialogCmd, kJTrue);

	if (!itsBuffer->IsEmpty())
		{
		flags.SetElement(kFindClipboardBackwardCmd, kJTrue);
		flags.SetElement(kFindClipboardForwardCmd,  kJTrue);

		if (TEHasSearchText())
			{
			flags.SetElement(kFindNextCmd,     kJTrue);
			flags.SetElement(kFindPreviousCmd, kJTrue);

			if (itsType == kFullEditor)
				{
				flags.SetElement(kReplaceAllBackwardCmd, kJTrue);
				flags.SetElement(kReplaceAllForwardCmd,  kJTrue);

				if (HasSelection())
					{
					flags.SetElement(kReplaceSelectionCmd,      kJTrue);
					flags.SetElement(kReplaceFindNextCmd,       kJTrue);
					flags.SetElement(kReplaceFindPrevCmd,       kJTrue);
					flags.SetElement(kReplaceAllInSelectionCmd, kJTrue);
					}
				}
			}
		}

	if (HasSelection())
		{
		flags.SetElement(kEnterSearchTextCmd,       kJTrue);
		flags.SetElement(kEnterReplaceTextCmd,      kJTrue);
		flags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		flags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	return flags;
}

/******************************************************************************
 TEDraw (protected)

	Draw everything that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor::TEDraw
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	// shade the dead-zone
/*
	if (itsLeftMarginWidth > kMinLeftMarginWidth)
		{
		const JColorIndex savedColor = p.GetPenColor();
		const JBoolean savedFill     = p.IsFilling();
		p.SetPenColor(itsColormap->GetGray90Color());
		p.SetFilling(kJTrue);
		const JRect& clipRect = p.GetClipRect();
		p.Rect(0, clipRect.top, itsLeftMarginWidth-1, clipRect.height());
		p.SetPenColor(savedColor);
		p.SetFilling(savedFill);
		}
*/
	p.ShiftOrigin(itsLeftMarginWidth, 0);

	// draw the text

	TEDrawText(p, rect);

	// if DND, draw drop location and object being dragged

	if ((itsDragType == kDragAndDrop || itsDragType == kLocalDragAndDrop) &&
		itsDropLoc.charIndex > 0)
		{
		TEDrawCaret(p, itsDropLoc);

		if (itsDragType == kLocalDragAndDrop)
			{
			JRect r = CalcLocalDNDRect(itsPrevPt);
			r.Shift(-itsLeftMarginWidth, 0);
			p.SetPenColor(itsDragColor);
			p.Rect(r);
			}
		}

	// otherwise, draw insertion caret

	else if (itsActiveFlag && itsCaretVisibleFlag && itsSelection.IsEmpty() &&
			 itsType == kFullEditor)
		{
		TEDrawCaret(p, itsCaretLoc);
		}

	// clean up

	p.ShiftOrigin(-itsLeftMarginWidth, 0);
}

/******************************************************************************
 TEDrawText (private)

	Draw the text that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor::TEDrawText
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	if (IsEmpty())
		{
		return;
		}

	const JSize lineCount = GetLineCount();

	JCoordinate h;
	const JIndex startLine = CalcLineIndex(rect.top, &h);

	// draw selection region

	if (itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText)
		{
		TEDrawSelection(p, rect, startLine, h);
		}

	// draw text, one line at a time

	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(GetLineStart(startLine), &runIndex, &firstInRun);
	assert( found );

	JRunArrayIterator<LineGeometry> iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=lineCount; i++)
		{
		const JBoolean ok = iter.Next(&geom);
		assert( ok );

		TEDrawLine(p, h, geom, i, &runIndex, &firstInRun);

		h += geom.height;
		if (h >= rect.bottom)
			{
			break;
			}
		}
}

/******************************************************************************
 TEDrawLine (private)

	Draw the text on the given line.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

inline void
teDrawSpaces
	(
	const JCharacter*		buffer,
	const JRunArray<JFont>&	styles,
	const JIndex			startChar,
	const JInteger			direction,		// +1/-1
	const JIndex			trueRunEnd,
	JPainter&				p,
	const JCoordinate		left,
	const JCoordinate		ycenter,
	const JFont&			f,
	const JColorIndex		wsColor
	)
{
	JCoordinate l = left;
	JSize w       = f.GetCharWidth(' ');

	p.SetLineWidth(1);
	p.SetPenColor(wsColor);

	JIndex i = startChar;
	JPoint pt;
	while (buffer[i-1] == ' ')
		{
		if ((direction == +1 && i > trueRunEnd) ||
			(direction == -1 && i < trueRunEnd))
			{
			JFont f = styles.GetElement(i);
			w       = f.GetCharWidth(' ');
			}

		if (direction == -1)
			{
			l -= w;
			}
		pt.Set(l + w/2 - 1, ycenter);
		p.Point(pt);
		if (direction == +1)
			{
			l += w;
			}
		i += direction;
		}
}

void
JTextEditor::TEDrawLine
	(
	JPainter&			p,
	const JCoordinate	top,
	const LineGeometry&	geom,
	const JIndex		lineIndex,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
{
	TEDrawInMargin(p, JPoint(-itsLeftMarginWidth, top), geom, lineIndex);

	const JSize lineLength = GetLineLength(lineIndex);
	assert( lineLength > 0 );

	JBoolean lineEndsWithNewline = kJFalse;

	JIndex startChar = GetLineStart(lineIndex);
	JIndex endChar   = startChar + lineLength - 1;
	if (itsBuffer->GetCharacter(endChar) == '\n')	// some fonts draw stuff for \n
		{
		endChar--;
		lineEndsWithNewline = kJTrue;
		}

	JCoordinate left = 0;

	JBoolean wsInit = kJFalse;
	JRect wsRect;
	JCoordinate wsYCenter = 0;
	if (itsDrawWhitespaceFlag)
		{
		wsRect.Set(top+3, left, top+geom.height-4, left);
		if (wsRect.height() < 3)
			{
			wsRect.top    -= (3 - wsRect.height())/2;
			wsRect.bottom += 3 - wsRect.height();
			}
		wsRect.bottom -= (wsRect.height() % 2);
		wsRect.right  += wsRect.height();

		wsYCenter = wsRect.ycenter();
		}

	JString s;
	while (startChar <= endChar)
		{
		JSize runLength        = itsStyles->GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startChar - *firstInRun;
		if (startChar + runLength-1 > endChar)
			{
			runLength = endChar - startChar + 1;
			}

		const JFont& f = itsStyles->GetRunDataRef(*runIndex);
		s              = itsBuffer->GetSubstring(startChar, startChar + runLength-1);

		// If the line starts with spaces, we have to draw them.

		if (!wsInit && itsDrawWhitespaceFlag)
			{
			teDrawSpaces(*itsBuffer, *itsStyles, startChar, +1, trueRunEnd,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}
		wsInit = kJTrue;

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabIndex;
		if (s.LocateSubstring("\t", &tabIndex))
			{
			runLength = tabIndex - 1;
			if (runLength > 0)
				{
				s = itsBuffer->GetSubstring(startChar, startChar + runLength-1);
				}
			}
		else
			{
			tabIndex = 0;
			}

		if (runLength > 0)
			{
			p.SetFont(f);
			JCoordinate ascent,descent;
			p.GetLineHeight(&ascent, &descent);

			p.String(left, top + geom.ascent - ascent, s);

			// we only care if there is more text on the line

			if (startChar + runLength <= endChar || itsDrawWhitespaceFlag)
				{
				left += p.GetStringWidth(s);
				}
			}

		if (tabIndex > 0)
			{
			if (itsDrawWhitespaceFlag)
				{
				p.SetLineWidth(1);
				p.SetPenColor(itsWhitespaceColor);

				wsRect.left  = left;
				wsRect.right = left + wsRect.height();

				const JCoordinate xc = wsRect.xcenter();
				const JCoordinate yc = wsRect.ycenter();
				const JPoint top   (xc, wsRect.top);
				const JPoint left  (wsRect.left, yc);
				const JPoint bottom(xc, wsRect.bottom);
				const JPoint right (wsRect.right, yc);

				p.Line(left, right);
				p.Line(top, right);
				p.LineTo(bottom);
				}

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(*itsBuffer, *itsStyles,
							 startChar + runLength - 1, -1, *firstInRun,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}

			left += GetTabWidth(startChar + runLength, left);
			runLength++;

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(*itsBuffer, *itsStyles,
							 startChar + runLength, +1, trueRunEnd,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}
			}

		startChar += runLength;
		if (startChar > trueRunEnd)		// move to next style run
			{
			*firstInRun = startChar;
			(*runIndex)++;
			}
		}

	// account for newline

	if (lineEndsWithNewline)
		{
		if (itsDrawWhitespaceFlag)
			{
			p.SetLineWidth(1);
			p.SetPenColor(itsWhitespaceColor);

			JRect rect(top+3, left, top+geom.height-2, left);
			rect.right += rect.height();
			const JCoordinate xc = rect.xcenter();
			const JCoordinate yc = rect.ycenter();

			JCoordinate delta = rect.height()/4;
			if (delta < 1)
				{
				delta = 1;
				}

			const JPoint pt1(xc, rect.top);
			const JPoint pt2(xc, rect.bottom);
			const JPoint pt3(xc - delta, rect.bottom - delta);
			const JPoint pt4(xc, rect.bottom - 2*delta);

			p.Line(pt1, pt2);
			p.LineTo(pt3);
			p.LineTo(pt4);

			const JFont& f = itsStyles->GetRunDataRef(*runIndex);
			teDrawSpaces(*itsBuffer, *itsStyles, endChar, -1, *firstInRun,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}

		const JSize runLength = itsStyles->GetRunLength(*runIndex);
		if (startChar >= *firstInRun + runLength-1)
			{
			*firstInRun += runLength;
			(*runIndex)++;
			}
		}
}

/******************************************************************************
 TEDrawInMargin (virtual protected)

	Scribble in the margins, e.g., bookmarks, line numbers, etc.

 ******************************************************************************/

void
JTextEditor::TEDrawInMargin
	(
	JPainter&			p,
	const JPoint&		topLeft,
	const LineGeometry&	geom,
	const JIndex		lineIndex
	)
{
}

/******************************************************************************
 TEDrawSelection (private)

	Draw the selection region.

 ******************************************************************************/

void
JTextEditor::TEDrawSelection
	(
	JPainter&			p,
	const JRect&		rect,
	const JIndex		startVisLine,
	const JCoordinate	startVisLineTop
	)
{
	assert( itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText );

	// calculate intersection of selection region and drawing region

	JIndex startLine = GetLineForChar(itsSelection.first);
	JIndex endLine   = GetLineForChar(itsSelection.last);

	const JIndex origStartLine = startLine;
	const JIndex origEndLine   = endLine;

	JIndex startChar;
	JCoordinate x, y;

	JCoordinate endVisLineTop;
	const JIndex endVisLine = CalcLineIndex(rect.bottom, &endVisLineTop);
	if (startVisLine > endLine || endVisLine < startLine)
		{
		return;
		}
	else if (startVisLine > startLine)
		{
		startLine = startVisLine;
		startChar = GetLineStart(startVisLine);
		x         = 0;
		y         = startVisLineTop;
		}
	else
		{
		startChar = itsSelection.first;
		x         = GetCharLeft(CaretLocation(startChar, startLine));
		y         = GetLineTop(startLine);
		}

	if (endLine > endVisLine)
		{
		endLine = endVisLine;
		}

	// draw the selection region

	const JColorIndex savedColor = p.GetPenColor();
	const JBoolean savedFill     = p.IsFilling();
	if (itsSelActiveFlag)
		{
		p.SetPenColor(itsSelectionColor);
		p.SetFilling(kJTrue);
		}
	else
		{
		p.SetPenColor(itsSelectionOutlineColor);
		p.SetFilling(kJFalse);
		}

	const JCoordinate xmax = JMax(itsGUIWidth, itsWidth);

	JRunArrayIterator<LineGeometry> iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=endLine; i++)
		{
		const JBoolean ok = iter.Next(&geom);
		assert( ok );

		JCoordinate w;
		JIndex endChar = GetLineEnd(i);
		if (endChar > itsSelection.last)
			{
			endChar = itsSelection.last;
			w = GetStringWidth(startChar, endChar);
			}
		else
			{
			w = xmax - x;
			}

		const JCoordinate bottom = y + geom.height - 1;
		if (itsSelActiveFlag)
			{
			// solid rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (origStartLine == origEndLine)
			{
			// empty rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (i == origStartLine)
			{
			// top of selection region

			p.Line(0,bottom, x,bottom);
			p.LineTo(x,y);
			p.LineTo(x+w,y);
			p.LineTo(x+w,bottom);
			}
		else if (i == origEndLine)
			{
			// bottom of selection region

			p.Line(0,y, 0,bottom);
			p.LineTo(w,bottom);
			p.LineTo(w,y);
			p.LineTo(xmax,y);
			}
		else
			{
			// vertical sides of selection region

			p.Line(0,y, 0,bottom);
			p.Line(xmax,y, xmax,bottom);
			}

		startChar = endChar+1;

		x  = 0;		// all lines after the first start at the far left
		y += geom.height;
		}

	// clean up

	p.SetPenColor(savedColor);
	p.SetFilling(savedFill);
}

/******************************************************************************
 TEDrawCaret (private)

 ******************************************************************************/

void
JTextEditor::TEDrawCaret
	(
	JPainter&				p,
	const CaretLocation&	caretLoc
	)
{
	assert( itsType == kFullEditor );

	JCoordinate x, y1, y2;

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())
		{
		x  = -1;
		y1 = GetLineBottom(caretLoc.lineIndex) + 1;
		y2 = y1 + GetEWNHeight()-1;
		}
	else
		{
		x = GetCharLeft(caretLoc) - 1;
		if (x >= itsWidth)
			{
			x = itsWidth;		// keep inside bounds
			}

		y1 = GetLineTop(caretLoc.lineIndex);
		y2 = y1 + GetLineHeight(caretLoc.lineIndex)-1;
		}

	const JColorIndex savedColor = p.GetPenColor();
	p.SetPenColor(itsCaretColor);

	if (itsCaretMode == kBlockCaret)
		{
		JCoordinate w = 5;
		if (IndexValid(caretLoc.charIndex) &&
			itsBuffer->GetCharacter(caretLoc.charIndex) != '\n')
			{
			w = GetCharWidth(caretLoc);
			}
		p.Rect(x,y1, w+1,y2-y1);
		}
	else
		{
		p.Line(x,y1, x,y2);
		}

	p.SetPenColor(savedColor);
}

/******************************************************************************
 TEWillDragAndDrop (protected)

 ******************************************************************************/

JBoolean
JTextEditor::TEWillDragAndDrop
	(
	const JPoint&	pt,
	const JBoolean	extendSelection,
	const JBoolean	dropCopy
	)
	const
{
	if (itsActiveFlag && itsType != kStaticText && itsPerformDNDFlag &&
		!itsSelection.IsEmpty() && !extendSelection)
		{
		return PointInSelection(pt);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PointInSelection (protected)

 ******************************************************************************/

JBoolean
JTextEditor::PointInSelection
	(
	const JPoint& origPt
	)
	const
{
	if (HasSelection())
		{
		const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
		const CaretLocation caretLoc = CalcCaretLocation(pt);

		return JI2B(itsSelection.first <= caretLoc.charIndex &&
					caretLoc.charIndex <= itsSelection.last+1);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 TEHandleMouseDown (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseDown
	(
	const JPoint&	origPt,
	const JSize		clickCount,
	const JBoolean	extendSelection,
	const JBoolean	partialWord
	)
{
	assert( itsActiveFlag );

	itsDragType         = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;
	if (itsType == kStaticText)
		{
		return;
		}

	DeactivateCurrentUndo();

	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	const CaretLocation caretLoc = CalcCaretLocation(pt);

	itsStartPt = itsPrevPt = pt;

	if (!itsSelection.IsEmpty() && extendSelection &&
		caretLoc.charIndex <= itsSelection.first)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.last+1;
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (!itsSelection.IsEmpty() && extendSelection)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.first;
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (itsPerformDNDFlag && clickCount == 1 && !itsSelection.IsEmpty() &&
			 itsSelection.first <= caretLoc.charIndex &&
			 caretLoc.charIndex <= itsSelection.last+1)
		{
		itsDragType = kDragAndDrop;
		itsDropLoc  = CaretLocation(0,0);
		}
	else if (extendSelection)
		{
		itsDragType       = kSelectDrag;
		itsSelectionPivot = itsCaretLoc.charIndex;	// save this before SetSelection()

		if (caretLoc.charIndex < itsCaretLoc.charIndex)
			{
			SetSelection(caretLoc.charIndex, itsCaretLoc.charIndex-1);
			}
		else if (itsCaretLoc.charIndex < caretLoc.charIndex)
			{
			SetSelection(itsCaretLoc.charIndex, caretLoc.charIndex-1);
			}
		}
	else if (clickCount == 1)
		{
		if (caretLoc != itsCaretLoc)
			{
			SetCaretLocation(caretLoc);
			}
		itsDragType       = kSelectDrag;
		itsSelectionPivot = caretLoc.charIndex;
		}
	else if (clickCount == 2)
		{
		itsDragType = (partialWord ? kSelectPartialWordDrag : kSelectWordDrag);
		TEGetDoubleClickSelection(caretLoc.charIndex, partialWord, kJFalse,
								  &itsWordSelPivot);
		SetSelection(itsWordSelPivot);
		}
	else if (clickCount == 3)
		{
		itsDragType     = kSelectLineDrag;
		itsLineSelPivot = caretLoc.lineIndex;
		SetSelection(GetLineStart(caretLoc.lineIndex), GetLineEnd(caretLoc.lineIndex));
		}

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseDrag (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseDrag
	(
	const JPoint& origPt
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType == kInvalidDrag || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDrag(origPt);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	if (itsDragType == kSelectDrag && caretLoc.charIndex < itsSelectionPivot)
		{
		SetSelection(caretLoc.charIndex, itsSelectionPivot-1, kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectDrag && caretLoc.charIndex == itsSelectionPivot)
		{
		SetCaretLocation(caretLoc);
		}
	else if (itsDragType == kSelectDrag && caretLoc.charIndex > itsSelectionPivot)
		{
		SetSelection(itsSelectionPivot, caretLoc.charIndex-1, kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectWordDrag || itsDragType == kSelectPartialWordDrag)
		{
		JIndexRange range;
		TEGetDoubleClickSelection(caretLoc.charIndex,
								  JI2B(itsDragType == kSelectPartialWordDrag),
								  kJTrue, &range);
		SetSelection(JCovering(itsWordSelPivot, range), kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectLineDrag)
		{
		SetSelection(GetLineStart( JMin(itsLineSelPivot, caretLoc.lineIndex) ),
					 GetLineEnd(   JMax(itsLineSelPivot, caretLoc.lineIndex) ),
					 kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kLocalDragAndDrop)
		{
		TERefreshCaret(itsDropLoc);
		CaretLocation dropLoc = CaretLocation(0,0);
		if (caretLoc.charIndex  <= itsSelection.first ||
			itsSelection.last+1 <= caretLoc.charIndex)
			{
			dropLoc = caretLoc;	// only drop outside selection
			}
		itsDropLoc = dropLoc;
		TERefreshCaret(itsDropLoc);
		TERefreshRect( CalcLocalDNDRect(itsPrevPt) );
		TERefreshRect( CalcLocalDNDRect(pt) );
		}
	else if (itsDragType == kDragAndDrop && JMouseMoved(itsStartPt, pt))
		{
		itsDragType = kInvalidDrag;		// wait for TEHandleDNDEnter()
		if (TEBeginDND())
			{
			itsIsDragSourceFlag = kJTrue;
			// switches to TEHandleDND*()
			}
		else if (itsType == kFullEditor)
			{
			itsDragType = kLocalDragAndDrop;
			}
		}

	itsPrevPt = pt;

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseUp (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseUp
	(
	const JBoolean dropCopy
	)
{
	// handle local DND

	if (itsDragType == kLocalDragAndDrop && itsDropLoc.charIndex != 0)
		{
		const JIndex charIndex = itsDropLoc.charIndex;
		itsDropLoc.charIndex   = 0;		// no longer valid, so don't let TEDraw() draw it
		itsDropLoc.lineIndex   = 0;
		DropSelection(charIndex, dropCopy);
		TERefreshRect( CalcLocalDNDRect(itsPrevPt) );	// get rid of rectangle
		}
	else if (itsDragType == kLocalDragAndDrop || itsDragType == kDragAndDrop)
		{
		const CaretLocation startLoc = CalcCaretLocation(itsStartPt);
		if (CalcCaretLocation(itsPrevPt) == startLoc)
			{
			SetCaretLocation(startLoc);
			}
		}

	// set itsSelectionPivot for "shift-arrow" selection

	else if (!itsSelection.IsEmpty() &&
			 (itsDragType == kSelectDrag ||
			  itsDragType == kSelectPartialWordDrag ||
			  itsDragType == kSelectWordDrag ||
			  itsDragType == kSelectLineDrag))
		{
		if ((itsDragType == kSelectWordDrag ||
			 itsDragType == kSelectPartialWordDrag) &&
			itsWordSelPivot.last  == itsSelection.last &&
			itsWordSelPivot.first != itsSelection.first)
			{
			itsSelectionPivot = itsSelection.last+1;
			}
		else if (itsDragType == kSelectLineDrag &&
				 GetLineEnd(itsLineSelPivot)   == itsSelection.last &&
				 GetLineStart(itsLineSelPivot) != itsSelection.first)
			{
			itsSelectionPivot = itsSelection.last+1;
			}

		if (itsSelectionPivot == itsSelection.last+1)
			{
			itsCaretX = GetCharLeft(CalcCaretLocation(itsSelection.first));
			}
		else
			{
			itsSelectionPivot = itsSelection.first;
			itsCaretX = GetCharRight(CalcCaretLocation(itsSelection.last));
			}
		}

	// save drag type for "extend" drag

	if (itsDragType == kSelectDrag ||
		itsDragType == kSelectPartialWordDrag ||
		itsDragType == kSelectWordDrag ||
		itsDragType == kSelectLineDrag)
		{
		itsPrevDragType = itsDragType;
		}
	else
		{
		itsPrevDragType = kInvalidDrag;
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDEnter (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDEnter()
{
	itsDragType = kDragAndDrop;
	itsDropLoc  = CaretLocation(0,0);
	TERefreshCaret(itsCaretLoc);		// hide the typing caret
}

/******************************************************************************
 TEHandleDNDHere (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDHere
	(
	const JPoint&	origPt,
	const JBoolean	dropOnSelf
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType != kDragAndDrop || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDND(origPt);
	TERefreshCaret(itsDropLoc);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	CaretLocation dropLoc = CaretLocation(0,0);
	if (!dropOnSelf ||
		caretLoc.charIndex <= itsSelection.first ||
		itsSelection.last+1 <= caretLoc.charIndex)
		{
		dropLoc = caretLoc;		// only drop outside selection
		}
	itsDropLoc = dropLoc;
	TERefreshCaret(itsDropLoc);

	itsPrevPt = pt;
	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleDNDLeave (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDLeave()
{
	assert( itsDragType == kDragAndDrop );

	TERefreshCaret(itsDropLoc);
	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDDrop (protected)

	If we are not the source, the derived class has to figure out how to get
	the data from the target.  Once it has the data, it should call
	Paste(text,style).

 ******************************************************************************/

void
JTextEditor::TEHandleDNDDrop
	(
	const JPoint&	pt,
	const JBoolean	dropOnSelf,
	const JBoolean	dropCopy
	)
{
	assert( itsDragType == kDragAndDrop );

	TEHandleDNDHere(pt, dropOnSelf);
	TERefreshCaret(itsDropLoc);

	if (dropOnSelf)
		{
		itsDragType = kLocalDragAndDrop;
		TEHandleMouseUp(dropCopy);
		}
	else
		{
		SetCaretLocation(itsDropLoc);
		TEPasteDropData();

		const JIndex index = GetInsertionIndex();
		if (index > itsDropLoc.charIndex)
			{
			SetSelection(itsDropLoc.charIndex, index-1);
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 CalcLocalDNDRect (private)

	Returns the rectangle to draw when performing local DND.
	The point should be relative to the left margin.
	The rectangle is relative to the frame.

 ******************************************************************************/

JRect
JTextEditor::CalcLocalDNDRect
	(
	const JPoint& pt
	)
	const
{
	JRect r(pt, pt);
	r.Shift(itsLeftMarginWidth, 0);
	r.Shrink(-kDraggedOutlineRadius, -kDraggedOutlineRadius);
	return r;
}

/******************************************************************************
 DropSelection (private)

	Called by TEHandleMouseUp() and JTEUndoDrop.  We modify the undo
	information even though we are private because we implement a single
	user command.

 ******************************************************************************/

void
JTextEditor::DropSelection
	(
	const JIndex	origDropLoc,
	const JBoolean	dropCopy
	)
{
	assert( !itsSelection.IsEmpty() );

	itsIsDragSourceFlag = kJFalse;		// allow text to be modified

	if (!dropCopy &&
		(origDropLoc == itsSelection.first ||
		 origDropLoc == itsSelection.last + 1))
		{
		return;
		}

	const JSize textLen = itsSelection.GetLength();

	JString dropText;
	JRunArray<JFont> dropStyles;
	const JBoolean ok = Copy(&dropText, &dropStyles);
	assert( ok );

	JIndex dropLoc       = origDropLoc;
	JTEUndoBase* newUndo = NULL;
	if (dropCopy)
		{
		SetCaretLocation(dropLoc);
		newUndo = jnew JTEUndoPaste(this, textLen);
		}
	else
		{
		JIndex origIndex = itsSelection.first;
		if (dropLoc > itsSelection.first)
			{
			dropLoc -= textLen;
			}
		else
			{
			assert( dropLoc < itsSelection.first );
			origIndex += textLen;
			}

		newUndo = jnew JTEUndoDrop(this, origIndex, dropLoc, textLen);

		DeleteText(itsSelection);
		Recalc(itsSelection.first, 1, kJTrue, kJFalse);
		}
	assert( newUndo != NULL );

	itsSelection.SetToNothing();
	const JSize pasteLength = InsertText(dropLoc, dropText, &dropStyles);
	newUndo->SetPasteLength(pasteLength);

	Recalc(dropLoc, textLen, kJFalse, kJFalse);
	SetSelection(dropLoc, dropLoc + textLen-1);
	TEScrollToSelection(kJFalse);

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 TEGetDoubleClickSelection (private)

	Select the word that was clicked on.  By computing the end of the word
	found by GetWordStart(), we avoid selecting two words when the user
	double clicks on the space between them.

 ******************************************************************************/

void
JTextEditor::TEGetDoubleClickSelection
	(
	const JIndex	charIndex,
	const JBoolean	partialWord,
	const JBoolean	dragging,
	JIndexRange*	range
	)
{
	JIndex startIndex, endIndex;
	if (partialWord)
		{
		startIndex = GetPartialWordStart(charIndex);
		endIndex   = GetPartialWordEnd(startIndex);
		}
	else
		{
		startIndex = GetWordStart(charIndex);
		endIndex   = GetWordEnd(startIndex);
		}

	// To select the word, the caret location should be inside the word
	// or on the character following the word.

	const JSize bufLength = itsBuffer->GetLength();

	if ((startIndex <= charIndex && charIndex <= endIndex) ||
		(!dragging &&
		 ((charIndex == endIndex+1  && IndexValid(endIndex+1)) ||
		  (charIndex == bufLength+1 && endIndex == bufLength))) )
		{
		range->Set(startIndex, endIndex);
		}

	// Otherwise, we select the character that was clicked on

	else
		{
		range->first = range->last = JMin(charIndex, bufLength);
		if (range->first > 1 &&
			itsBuffer->GetCharacter(range->first)   == '\n' &&
			itsBuffer->GetCharacter(range->first-1) != '\n')
			{
			(range->first)--;
			(range->last)--;
			}
		}
}

/******************************************************************************
 TEHitSamePart

 ******************************************************************************/

JBoolean
JTextEditor::TEHitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	const CaretLocation loc1 = CalcCaretLocation(pt1);
	const CaretLocation loc2 = CalcCaretLocation(pt2);
	return JConvertToBoolean(loc1 == loc2);
}

/******************************************************************************
 SetKeyHandler

 ******************************************************************************/

void
JTextEditor::SetKeyHandler
	(
	JTEKeyHandler* handler
	)
{
	jdelete itsKeyHandler;
	itsKeyHandler = handler;
	if (itsKeyHandler != NULL)
		{
		itsKeyHandler->Initialize();
		}
}

/******************************************************************************
 TEHandleKeyPress (protected)

	Returns kJTrue if the key was processed.

 ******************************************************************************/

JBoolean
JTextEditor::TEHandleKeyPress
	(
	const JCharacter	origKey,
	const JBoolean		selectText,
	const CaretMotion	motion,
	const JBoolean		deleteToTabStop
	)
{
	assert( itsActiveFlag );
	assert( (!itsSelection.IsEmpty() && itsCaretLoc.charIndex == 0) ||
			( itsSelection.IsEmpty() && itsCaretLoc.charIndex >  0) );

	if (itsType == kStaticText)
		{
		return kJFalse;
		}
	else if (origKey == kJEscapeKey && itsDragType == kLocalDragAndDrop)
		{
		itsDragType = kInvalidDrag;
		TERefresh();
		return kJTrue;
		}
	else if (TEIsDragging())
		{
		return kJTrue;
		}

	// pre-processing

	JCharacter key = origKey;
	if (key == '\r')
		{
		key = '\n';
		}

	// overrides

	if (itsKeyHandler != NULL &&
		itsKeyHandler->HandleKeyPress(key, selectText, motion, deleteToTabStop))
		{
		return kJTrue;
		}

	const JSize bufLength = itsBuffer->GetLength();

	// We select text by selecting to where the caret ends up.

	const JBoolean isArrowKey = JI2B(
		key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow   || key == kJDownArrow);
	const JBoolean willSelectText = JI2B( selectText && isArrowKey );

	if (willSelectText)
		{
		JBoolean restoreCaretX        = kJTrue;
		const JCoordinate savedCaretX = itsCaretX;

		if (!itsSelection.IsEmpty() && itsSelectionPivot == itsSelection.last+1)
			{
			SetCaretLocation(itsSelection.first);
			}
		else if (!itsSelection.IsEmpty() && itsSelectionPivot == itsSelection.first)
			{
			SetCaretLocation(itsSelection.last+1);
			}
		else if (!itsSelection.IsEmpty())	// SetSelection() was called by outsider
			{
			itsSelectionPivot = itsSelection.first;
			restoreCaretX     = kJFalse;
			SetCaretLocation(itsSelection.last+1);
			}
		else
			{
			itsSelectionPivot = itsCaretLoc.charIndex;
			}

		if (restoreCaretX && (key == kJUpArrow || key == kJDownArrow))
			{
			itsCaretX = savedCaretX;
			}
		}
	else if (itsType == kSelectableText && !isArrowKey)
		{
		return kJFalse;
		}

	JBoolean processed = kJTrue;

	// left arrow

	if (key == kJLeftArrow && motion == kMoveByLine)
		{
		GoToBeginningOfLine();
		}

	else if (key == kJLeftArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetWordStart(itsSelection.first));				// works for zero
		}
	else if (key == kJLeftArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetWordStart(itsCaretLoc.charIndex-1));		// works for zero
		}

	else if (key == kJLeftArrow && motion == kMoveByPartialWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetPartialWordStart(itsSelection.first));		// works for zero
		}
	else if (key == kJLeftArrow && motion == kMoveByPartialWord)
		{
		SetCaretLocation(GetPartialWordStart(itsCaretLoc.charIndex-1));	// works for zero
		}

	else if (key == kJLeftArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.first);
		}
	else if (key == kJLeftArrow)
		{
		if (itsCaretLoc.charIndex > 1)
			{
			SetCaretLocation(itsCaretLoc.charIndex-1);
			}
		else
			{
			SetCaretLocation(1);	// scroll to it
			}
		}

	// right arrow

	else if (key == kJRightArrow && motion == kMoveByLine)
		{
		GoToEndOfLine();
		}

	else if (key == kJRightArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetWordEnd(itsSelection.last)+1);
		}
	else if (key == kJRightArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetWordEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJRightArrow && motion == kMoveByPartialWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetPartialWordEnd(itsSelection.last)+1);
		}
	else if (key == kJRightArrow && motion == kMoveByPartialWord)
		{
		SetCaretLocation(GetPartialWordEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJRightArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.last+1);
		}
	else if (key == kJRightArrow)
		{
		if (itsCaretLoc.charIndex <= bufLength)
			{
			SetCaretLocation(itsCaretLoc.charIndex+1);
			}
		else
			{
			SetCaretLocation(bufLength+1);	// scroll to it
			}
		}

	// up arrow

	else if (key == kJUpArrow && motion == kMoveByLine)
		{
		SetCaretLocation(1);
		}

	else if (key == kJUpArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetParagraphStart(itsSelection.first-1));
		}
	else if (key == kJUpArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetParagraphStart(itsCaretLoc.charIndex-1));
		}

	else if (key == kJUpArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.first);
		}
	else if (key == kJUpArrow && itsCaretLoc.charIndex == bufLength+1 &&
			 EndsWithNewline())
		{
		SetCaretLocation(GetLineStart(itsCaretLoc.lineIndex));
		}
	else if (key == kJUpArrow && itsCaretLoc.lineIndex > 1)
		{
		MoveCaretVert(-1);
		}
	else if (key == kJUpArrow)
		{
		SetCaretLocation(1);
		}

	// down arrow

	else if (key == kJDownArrow && motion == kMoveByLine)
		{
		SetCaretLocation(bufLength+1);
		}

	else if (key == kJDownArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetParagraphEnd(itsSelection.last+1)+1);
		}
	else if (key == kJDownArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetParagraphEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJDownArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.last+1);
		}
	else if (key == kJDownArrow && itsCaretLoc.lineIndex < GetLineCount())
		{
		MoveCaretVert(+1);
		}
	else if (key == kJDownArrow)
		{
		SetCaretLocation(bufLength+1);
		}

	// delete

	else if (key == kJDeleteKey && !itsSelection.IsEmpty())
		{
		DeleteSelection();
		}
	else if (key == kJDeleteKey && itsCaretLoc.charIndex > 1)
		{
		BackwardDelete(deleteToTabStop);
		}

	// forward delete

	else if (key == kJForwardDeleteKey && !itsSelection.IsEmpty())
		{
		DeleteSelection();
		}
	else if (key == kJForwardDeleteKey && itsCaretLoc.charIndex <= bufLength)
		{
		ForwardDelete(deleteToTabStop);
		}

	// insert character

	else if (itsTabToSpacesFlag && key == '\t')
		{
		InsertSpacesForTab();
		}

	else if (JIsPrint(key) || key == '\n' || key == '\t')
		{
		InsertKeyPress(key);
		}

	else
		{
		processed = kJFalse;
		}

	// finish selection process

	if (willSelectText)
		{
		const CaretLocation savedCaretLoc = itsCaretLoc;
		if (itsCaretLoc.charIndex < itsSelectionPivot)
			{
			SetSelection(itsCaretLoc.charIndex, itsSelectionPivot-1, kJFalse);
			}
		else if (itsCaretLoc.charIndex > itsSelectionPivot)
			{
			SetSelection(itsSelectionPivot, itsCaretLoc.charIndex-1, kJFalse);
			}

		itsPrevDragType = kSelectDrag;

		// show moving end of selection

		BroadcastCaretMessages(savedCaretLoc, kJTrue);
		}

	// We redraw the display immediately because it is very disconcerting
	// when the display does not instantly show the changes.

	TEUpdateDisplay();
	return processed;
}

/******************************************************************************
 InsertKeyPress (private)

 ******************************************************************************/

void
JTextEditor::InsertKeyPress
	(
	const JCharacter key
	)
{
	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(&isNew);
	typingUndo->HandleCharacter();

	const JBoolean hadSelection = !itsSelection.IsEmpty();
	if (hadSelection)
		{
		itsInsertionFont = itsStyles->GetElement(itsSelection.first);
		DeleteText(itsSelection);
		itsCaretLoc = CalcCaretLocation(itsSelection.first);
		itsSelection.SetToNothing();
		}
	const JCharacter s[2]   = { key, '\0' };
	const JSize pasteLength = InsertText(itsCaretLoc, s);
	assert( pasteLength == 1 );
	Recalc(itsCaretLoc, 1, hadSelection, kJFalse);
	SetCaretLocation(itsCaretLoc.charIndex+1);

	typingUndo->Activate();		// cancel SetCaretLocation()

	if (key == '\n' && itsAutoIndentFlag)
		{
		AutoIndent(typingUndo);
		}

	NewUndo(typingUndo, isNew);
}

/******************************************************************************
 BackwardDelete (private)

	Delete characters in front of the insertion caret.

 ******************************************************************************/

void
JTextEditor::BackwardDelete
	(
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	JIndex startIndex           = itsCaretLoc.charIndex-1;
	const JIndex origStartIndex = startIndex;
	JSize deleteLength;
	if (deleteToTabStop &&
		(itsBuffer->GetCharacter(startIndex) == ' ' ||
		 itsBuffer->GetCharacter(startIndex) == '\t'))
		{
		JIndex textColumn = GetColumnForChar(itsCaretLoc);
		deleteLength      = (textColumn-1) % itsCRMTabCharCount;
		if (deleteLength == 0)
			{
			deleteLength = itsCRMTabCharCount;

			for (JIndex i=1; i<=deleteLength; i++)
				{
				if (i > 1)
					{
					startIndex--;
					}

				const JCharacter c = itsBuffer->GetCharacter(startIndex);
				if (c == ' ')
					{
					textColumn--;
					}
				else if (c == '\t')
					{
					JSize tabWidth = CRMGetTabWidth(GetColumnForChar(CaretLocation(startIndex, itsCaretLoc.lineIndex))-1);
					textColumn    -= tabWidth;
					deleteLength  -= (tabWidth-1);
					}
				else	// normal delete when close to text
					{
					startIndex   = origStartIndex;
					deleteLength = 1;
					break;
					}
				}
			}
		else			// normal delete when close to text
			{
			deleteLength = 1;
			}
		}
	else
		{
		deleteLength = 1;
		}

	JIndexRange deleteRange(startIndex, startIndex + deleteLength - 1);
	if (returnText != NULL)
		{
		returnText->Set(*itsBuffer, deleteRange);
		}
	if (returnStyle != NULL)
		{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, deleteRange);
		}

	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(&isNew);
x	typingUndo->HandleDelete(deleteRange.first, deleteRange.last);

	const JFont f = itsStyles->GetElement(startIndex);	// preserve font
	DeleteText(deleteRange);
	Recalc(startIndex, 1, kJTrue, kJFalse);
	SetCaretLocation(startIndex);
	if (itsPasteStyledTextFlag)
		{
		itsInsertionFont = f;
		}

	typingUndo->Activate();		// cancel SetCaretLocation()
	NewUndo(typingUndo, isNew);
}

/******************************************************************************
 ForwardDelete (private)

	Delete characters following the insertion caret.

 ******************************************************************************/

void
JTextEditor::ForwardDelete
	(
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	JSize deleteLength;
	if (deleteToTabStop &&
		itsBuffer->GetCharacter(itsCaretLoc.charIndex) == ' ')
		{
		JIndex textColumn = GetColumnForChar(itsCaretLoc);
		deleteLength      = (textColumn-1) % itsCRMTabCharCount;
		if (deleteLength == 0)
			{
			deleteLength = itsCRMTabCharCount;

			for (JIndex i=1; i<=deleteLength; i++)
				{
				const JCharacter c = itsBuffer->GetCharacter(itsCaretLoc.charIndex+i-1);
				if (c == '\t')
					{
					deleteLength = i;
					break;
					}
				else if (c != ' ')	// normal delete when close to text
					{
					deleteLength = 1;
					break;
					}
				}
			}
		else						// normal delete when close to text
			{
			deleteLength = 1;
			}
		}
	else
		{
		deleteLength = 1;
		}

	JIndexRange deleteRange(itsCaretLoc.charIndex, itsCaretLoc.charIndex + deleteLength - 1);
	if (returnText != NULL)
		{
		returnText->Set(*itsBuffer, deleteRange);
		}
	if (returnStyle != NULL)
		{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, deleteRange);
		}

	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(&isNew);
x	typingUndo->HandleFwdDelete(deleteRange.first, deleteRange.last);

	DeleteText(deleteRange);
	Recalc(itsCaretLoc, 1, kJTrue, kJFalse);
	SetCaretLocation(itsCaretLoc.charIndex);

	typingUndo->Activate();		// cancel SetCaretLocation()
	NewUndo(typingUndo, isNew);
}

/******************************************************************************
 AutoIndent (private)

	Insert the "rest" prefix based on the previous line.  If the previous
	line is nothing but whitespace, clear it.

 ******************************************************************************/

void
JTextEditor::AutoIndent
	(
	JTEUndoTyping* typingUndo
	)
{
	assert( itsSelection.IsEmpty() );

	// Move up one line if we are not at the very end of the text.

	JIndex lineIndex = itsCaretLoc.lineIndex;
	if (itsCaretLoc.charIndex <= itsBuffer->GetLength())	// ends with newline
		{
		lineIndex--;
		}

	// Skip past lines that were created by wrapping the text.
	// (This is faster than using GetParagraphStart() because then we would
	//  have to call GetLineForChar(), which is a search.  GetLineStart()
	//  is an array lookup.)

	JIndex firstChar = GetLineStart(lineIndex);
	while (lineIndex > 1 && itsBuffer->GetCharacter(firstChar-1) != '\n')
		{
		lineIndex--;
		firstChar = GetLineStart(lineIndex);
		}

	// Calculate the prefix range for the original line.

	const JIndex lastChar = GetParagraphEnd(firstChar) - 1;
	assert( itsBuffer->GetCharacter(lastChar+1) == '\n' );

	JIndex firstTextChar = firstChar;
	JString linePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (lastChar < firstChar ||
		!CRMGetPrefix(&firstTextChar, lastChar,
					  &linePrefix, &prefixLength, &ruleIndex))
		{
		firstTextChar = lastChar+1;
		if (firstTextChar > firstChar)
			{
			linePrefix = itsBuffer->GetSubstring(firstChar, firstTextChar-1);
			}
		}
	else if (CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
		{
		// CRMBuildRestPrefix() will complain if we pass in the entire
		// line, so we give it only the whitespace.

		CRMRuleList* savedRuleList = itsCRMRuleList;
		itsCRMRuleList             = NULL;
		firstTextChar              = firstChar;
		ruleIndex                  = 0;
		CRMGetPrefix(&firstTextChar, lastChar,
					 &linePrefix, &prefixLength, &ruleIndex);
		itsCRMRuleList = savedRuleList;
		}

	// Generate the prefix and include it in the undo object.

	if (firstTextChar > firstChar)
		{
		linePrefix   = CRMBuildRestPrefix(linePrefix, ruleIndex, &prefixLength);
		prefixLength = linePrefix.GetLength();

		typingUndo->HandleCharacters(prefixLength);

		const JSize pasteLength = InsertText(itsCaretLoc, linePrefix);
		assert( pasteLength == prefixLength );
		Recalc(itsCaretLoc, prefixLength, kJFalse, kJFalse);

		JIndex newCaretChar = itsCaretLoc.charIndex + prefixLength;

		// check the initial whitespace range

		JIndex lastWSChar = firstChar;
		while (1)
			{
			const JCharacter c = itsBuffer->GetCharacter(lastWSChar);
			if ((c != ' ' && c != '\t') || lastWSChar > lastChar)		// can't use isspace() because '\n' stops us
				{
				lastWSChar--;
				break;
				}
			lastWSChar++;
			}

		// if the line is blank, clear it

		if (lastWSChar >= firstChar && itsBuffer->GetCharacter(lastWSChar+1) == '\n')
			{
x			typingUndo->HandleAutoIndentDelete(firstChar, lastWSChar);

			DeleteText(firstChar, lastWSChar);
			Recalc(CaretLocation(firstChar, lineIndex), 1, kJTrue, kJFalse);

			newCaretChar -= lastWSChar - firstChar + 1;
			}

		SetCaretLocation(newCaretChar);

		// re-activate undo after SetCaretLocation()

		typingUndo->Activate();
		}
}

/******************************************************************************
 InsertSpacesForTab (private)

	Insert spaces to use up the same amount of space that a tab would use.

 ******************************************************************************/

void
JTextEditor::InsertSpacesForTab()
{
	const JSize charIndex  = GetInsertionIndex();
	JSize lineIndex        = GetLineForChar(charIndex);
	JSize column           = charIndex - GetLineStart(lineIndex);

	if (charIndex == itsBuffer->GetLength()+1 && EndsWithNewline())
		{
		lineIndex++;
		column = 0;
		}

	const JSize spaceCount = CRMGetTabWidth(column);

	JString space;
	for (JIndex i=1; i<=spaceCount; i++)
		{
		space.AppendCharacter(' ');
		}
	Paste(space);
}

/******************************************************************************
 ContainsIllegalChars (static)

	Returns kJTrue if the given text contains characters that we will not
	accept:  00-08, 0B, 0E-1F, 7F

	We accept form feed (0C) because PrintPlainText() converts it to space.

	We accept all characters above 0x7F because they provide useful
	(though hopelessly system dependent) extensions to the character set.

 ******************************************************************************/

static const JRegex illegalCharRegex = "[\\0\x01-\x08\x0B\x0E-\x1F\x7F]+";

JBoolean
JTextEditor::ContainsIllegalChars
	(
	const JString&	text
	)
{
	return illegalCharRegex.Match(text);
}

/******************************************************************************
 RemoveIllegalChars (static)

	Returns kJTrue if we had to remove any characters that
	ContainsIllegalChars() would flag.

	If the user cancels, we toss the entire string and return kJTrue.

	style can be NULL or empty.

 ******************************************************************************/

JBoolean
JTextEditor::RemoveIllegalChars
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	assert( style == NULL || style->IsEmpty() ||
			style->GetElementCount() == text->GetLength() );

	const JSize origTextLength = text->GetLength();

	JString tmpText;
	tmpText.SetBlockSize(origTextLength+256);

	JRunArray<JFont> tmpStyle;
	if (style != NULL && !style->IsEmpty())
		{
		tmpStyle.SetBlockSize(style->GetRunCount()+16);
		}

	JIndexRange remainder(1, origTextLength);
	JIndexRange illegal;
	JIndex i, j = 1;
	while (illegalCharRegex.MatchWithin(*text, remainder, &illegal))
		{
		i = illegal.first;

		tmpText.Append(text->GetCString() + (j-1), i - j);
		if (style != NULL && !style->IsEmpty() && i > j)
			{
			tmpStyle.AppendSlice(*style, j, i-1);
			}

		remainder.first = j = illegal.last + 1;
		}

	if (j <= origTextLength)
		{
		i = origTextLength;
		tmpText.Append(text->GetCString() + (j-1), i - j + 1);
		if (style != NULL && !style->IsEmpty())
			{
			tmpStyle.AppendSlice(*style, j, i);
			}
		}

	*text = tmpText;
	if (style != NULL && !style->IsEmpty())
		{
		*style = tmpStyle;
		}

	return JConvertToBoolean( text->GetLength() < origTextLength );
}

/******************************************************************************
 GetWordStart (protected)

	Return the index of the first character of the word at the given location.
	This function is required to work for charIndex == 0.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || index.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	const JIndex charIndex = JMin(index.charIndex, itsBuffer.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsBuffer.GetByteCount());

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartAfter, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, kJFalse) && !IsCharacterInWord(c))
		{
		while (iter.Prev(&c) && !IsCharacterInWord(c))
			{
			// find end of word
			}
		}

	while (iter.Prev(&c) && IsCharacterInWord(c))
		{
		// find start of word
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetWordEnd

	Return the index of the last character of the word at the given location.
	This function is required to work for charIndex > buffer length.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, kJFalse) && !IsCharacterInWord(c))
		{
		while (iter.Next(&c) && !IsCharacterInWord(c))
			{
			// find start of word
			}
		}

	while (iter.Next(&c) && IsCharacterInWord(c))
		{
		// find end of word
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetCharacterInWordFunction

 ******************************************************************************/

JCharacterInWordFn
JTextEditor::GetCharacterInWordFunction()
	const
{
	return itsCharInWordFn;
}

/******************************************************************************
 SetCharacterInWordFunction

	Set the function that determines if a character is part of a word.
	The default is JIsAlnum(), which uses [A-Za-z0-9].

 ******************************************************************************/

void
JTextEditor::SetCharacterInWordFunction
	(
	JCharacterInWordFn f
	)
{
	assert( f != NULL );
	itsCharInWordFn = f;
}

/******************************************************************************
 IsCharacterInWord (protected)

	Returns kJTrue if the given character should be considered part of a word.

 ******************************************************************************/

JBoolean
JTextEditor::IsCharacterInWord
	(
	const JUtf8Character& c
	)
	const
{
	return itsCharInWordFn(c);
}

/******************************************************************************
 DefaultIsCharacterInWord (static private)

	Returns kJTrue if the given character should be considered part of a word.
	The definition is alphanumeric or apostrophe or underscore.

	This is not a virtual function because one needs to be able to set it
	for simple objects like input fields.

 ******************************************************************************/

JBoolean
JTextEditor::DefaultIsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return JI2B( c.IsAlnum() || c == '\'' || c == '_' );
}

/******************************************************************************
 GetPartialWordStart

	Return the index of the first character of the partial word at the given
	location.  This function is required to work for charIndex == 0.

	Example:  get_word Get142TheWordABCGood ABCDe
			  ^   ^    ^  ^  ^  ^   ^  ^    ^  ^
 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetPartialWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	const JIndex charIndex = JMin(index.charIndex, itsBuffer.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsBuffer.GetByteCount());

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartAfter, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, kJFalse) && !c.IsAlnum())
		{
		while (iter.Prev(&c) && !c.IsAlnum())
			{
			// find end of word
			}
		}
	else if (!iter.Prev(&c))
		{
		return TextIndex(1,1);
		}

	JUtf8Character prev = c;
	JBoolean foundLower = prev.IsLower();
	while (iter.Prev(&c))
		{
		foundLower = JI2B(foundLower || c.IsLower());
		if (!c.IsAlnum() ||
			(prev.IsUpper() && c.IsLower()) ||
			(prev.IsUpper() && c.IsUpper() && foundLower) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()))
			{
			iter.SkipNext();
			break;
			}

		prev = c;
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetPartialWordEnd

	Return the index of the last character of the partial word at the given
	location.  This function is required to work for charIndex > buffer length.

	Example:  get_word Get142TheWordABCGood
				^    ^   ^  ^  ^   ^  ^   ^
 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetPartialWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, kJFalse) && !c.IsAlnum())
		{
		while (iter.Next(&c) && !c.IsAlnum())
			{
			// find start of word
			}
		}
	else if (!iter.Next(&c))
		{
		return TextIndex(1,1);
		}

	JUtf8Character prev = c, c2;
	while (iter.Next(&c))
		{
		if (!c.IsAlnum() ||
			(prev.IsLower() && c.IsUpper()) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()) ||
			(iter.Next(&c2, kJFalse) &&
			 prev.IsUpper() && c.IsUpper() && c2.IsLower()))
			{
			break;
			}

		prev = c;
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphStart

	Return the index of the first character in the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex == 0.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetParagraphStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || index.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	while (iter.Prev(&c) && c != '\n')
		{
		// find start of paragraph
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphEnd

	Return the index of the newline that ends the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex > buffer length.

 ******************************************************************************/

JIndex
JTextEditor::GetParagraphEnd
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JUtf8Character c;
	while (iter.Next(&c) && c != '\n')
		{
		// find end of paragraph
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 SetCaretLocation

	Move the caret to the specified point.

 ******************************************************************************/

void
JTextEditor::SetCaretLocation
	(
	const JIndex origCharIndex
	)
{
	JIndex charIndex = JMax(origCharIndex, JIndex(1));
	charIndex        = JMin(charIndex, itsBuffer.GetCharacterCount()+1);

	SetCaretLocation( CalcCaretLocation(charIndex) );
}

// private

void
JTextEditor::SetCaretLocation
	(
	const CaretLocation& caretLoc
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	DeactivateCurrentUndo();

	const JBoolean hadSelection      = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc = itsCaretLoc;

	if (hadSelection)
		{
		TERefreshLines(GetLineForChar(itsSelection.first),
					   GetLineForChar(itsSelection.last));
		}

	itsSelection.SetToNothing();
	itsCaretLoc = caretLoc;
	itsCaretX   = GetCharLeft(itsCaretLoc);

	if (hadSelection || origCaretLoc != itsCaretLoc || !itsPasteStyledTextFlag)
		{
		itsInsertionFont = CalcInsertionFont(itsCaretLoc);
		}

	if (!TEScrollTo(itsCaretLoc))
		{
		TERefreshCaret(origCaretLoc);
		TERefreshCaret(itsCaretLoc);
		}

	BroadcastCaretMessages(itsCaretLoc,
		JI2B(hadSelection || origCaretLoc.lineIndex != itsCaretLoc.lineIndex));

	TECaretShouldBlink(kJTrue);
}

/******************************************************************************
 BroadcastCaretMessages (private)

 ******************************************************************************/

void
JTextEditor::BroadcastCaretMessages
	(
	const CaretLocation&	caretLoc,
	const JBoolean			lineChanged
	)
{
	if (!BroadcastCaretPosChanged(caretLoc) && lineChanged)
		{
		JIndex line = caretLoc.lineIndex;
		if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
			EndsWithNewline())
			{
			line++;
			}
		Broadcast(CaretLineChanged(line));
		}
}

JBoolean
JTextEditor::BroadcastCaretPosChanged
	(
	const CaretLocation& caretLoc
	)
{
	if (itsBcastLocChangedFlag)
		{
		JIndex line = caretLoc.lineIndex;
		JIndex col  = GetColumnForChar(caretLoc);
		if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
			EndsWithNewline())
			{
			line++;
			col = 1;
			}
		Broadcast(CaretLocationChanged(line, col));
		}
	return itsBcastLocChangedFlag;
}

/******************************************************************************
 GetColumnForChar

	Returns the column that the specified character is in.  If the caret is
	at the far left, it is column 1.

	Given that this is only useful with monospace fonts, the CRM tab width
	is used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

JIndex
JTextEditor::GetColumnForChar
	(
	const JIndex charIndex
	)
	const
{
	return GetColumnForChar(CalcCaretLocation(charIndex));
}

// protected

JIndex
JTextEditor::GetColumnForChar
	(
	const CaretLocation& caretLoc
	)
	const
{
	if (caretLoc.charIndex == itsBuffer->GetLength()+1 && EndsWithNewline())
		{
		return 1;
		}
	else
		{
		JIndex charIndex = GetLineStart(caretLoc.lineIndex);
		JIndex col       = 1;
		while (charIndex < caretLoc.charIndex && charIndex <= itsBuffer->GetLength())
			{
			if (itsBuffer->GetCharacter(charIndex) == '\t')
				{
				col += CRMGetTabWidth(col-1);
				}
			else
				{
				col++;
				}
			charIndex++;
			}

		return col;
		}
}

/******************************************************************************
 CharToByteRange (private)

	Optimized by starting JStringIterator at start of line, computed by
	using binary search.

 ******************************************************************************/

JUtf8ByteRange
JTextEditor::CharToByteRange
	(
	const JCharacterRange& charRange
	)
	const
{
	assert( !charRange.IsEmpty() );
	assert( text.RangeValid(charRange) );

	const TextIndex i = GetLineStart(GetLineForChar(charRange.first));

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, i.charIndex, i.byteIndex);

	return CharToByteRange(charRange, &iter);
}

/******************************************************************************
 CharToByteRange (static private)

	If JStringIterator is not AtBeginning(), assumes position is optimized.
	Otherwise, optimizes by starting JStringIterator at end closest to
	range.

 ******************************************************************************/

JUtf8ByteRange
JTextEditor::CharToByteRange
	(
	const JCharacterRange&	charRange,
	JStringIterator*		iter
	)
{
	
	if (iter->AtBeginning())
		{
		const JString& s = iter->GetString();
		const JSize d1   = charRange.first;
		const JSize d2   = s.GetCharacterCount() - charRange.last;
		if (d2 < d1)
			{
			iter->MoveTo(kJIteratorStartAtEnd, 0);
			}
		}

	JIndex i1, i2;
	if (iter->AtEnd())
		{
		iter.MoveTo(kJIteratorStartAfter, charRange.last);
		i2 = iter.GetPrevByteIndex();
		iter.MoveTo(kJIteratorStartBefore, charRange.first);
		i1 = iter.GetNextByteIndex();
		}
	else
		{
		iter.MoveTo(kJIteratorStartBefore, charRange.first);
		i1 = iter.GetNextByteIndex();
		iter.MoveTo(kJIteratorStartAfter, charRange.last);
		i2 = iter.GetPrevByteIndex();
		}

	return JUtf8ByteRange(i1, i2);
}

/******************************************************************************
 GoToColumn

 ******************************************************************************/

void
JTextEditor::GoToColumn
	(
	const JIndex lineIndex,
	const JIndex columnIndex
	)
{
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc(0, lineIndex);
	if (lineIndex > lineCount && EndsWithNewline())
		{
		caretLoc.charIndex = itsBuffer->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		caretLoc.charIndex    = GetLineStart(lineIndex);
		const JIndex endIndex = GetLineEnd(lineIndex);
		JIndex col            = 1;
		while (col < columnIndex && caretLoc.charIndex < endIndex)
			{
			if (itsBuffer->GetCharacter(caretLoc.charIndex) == '\t')
				{
				col += CRMGetTabWidth(col-1);
				}
			else
				{
				col++;
				}
			caretLoc.charIndex++;
			}
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToLine

 ******************************************************************************/

void
JTextEditor::GoToLine
	(
	const JIndex lineIndex
	)
{
	JIndex trueIndex      = lineIndex;
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc;
	if (trueIndex > lineCount && EndsWithNewline())
		{
		caretLoc.charIndex = itsBuffer->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		if (trueIndex == 0)
			{
			trueIndex = 1;
			}
		else if (trueIndex > lineCount)
			{
			trueIndex = lineCount;
			}
		caretLoc.charIndex = GetLineStart(trueIndex);
		caretLoc.lineIndex = trueIndex;
		}

	TEScrollToRect(CalcCaretRect(caretLoc), kJTrue);
	if (IsReadOnly())
		{
		SetSelection(GetLineStart(caretLoc.lineIndex),
					 GetLineEnd(caretLoc.lineIndex));
		}
	else
		{
		SetCaretLocation(caretLoc);
		}
}

/******************************************************************************
 SelectLine

 ******************************************************************************/

void
JTextEditor::SelectLine
	(
	const JIndex lineIndex
	)
{
	GoToLine(lineIndex);
	if (itsSelection.IsEmpty())
		{
		SetSelection(GetLineStart(itsCaretLoc.lineIndex),
					 GetLineEnd(itsCaretLoc.lineIndex));
		}
}

/******************************************************************************
 GoToBeginningOfLine

 ******************************************************************************/

void
JTextEditor::GoToBeginningOfLine()
{
	CaretLocation caretLoc;
	if (!itsSelection.IsEmpty())
		{
		caretLoc = CalcCaretLocation(itsSelection.first);
		}
	else
		{
		caretLoc = itsCaretLoc;
		}

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())
		{
		// set current value so we scroll to it
		}
	else if (itsMoveToFrontOfTextFlag && !itsBuffer->IsEmpty())
		{
		const JIndex firstChar = GetLineStart(caretLoc.lineIndex);
		JIndex lastChar        = GetLineEnd(caretLoc.lineIndex);
		if (itsBuffer->GetCharacter(lastChar) == '\n')
			{
			lastChar--;
			}

		JIndex firstTextChar = firstChar;
		JString linePrefix;
		JSize prefixLength;
		JIndex ruleIndex = 0;
		if (lastChar < firstChar ||
			!CRMGetPrefix(&firstTextChar, lastChar,
						  &linePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
			{
			firstTextChar = lastChar+1;
			}

		caretLoc.charIndex =
			caretLoc.charIndex <= firstTextChar ? firstChar : firstTextChar;
		}
	else
		{
		caretLoc.charIndex = GetLineStart(caretLoc.lineIndex);
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToEndOfLine

 ******************************************************************************/

void
JTextEditor::GoToEndOfLine()
{
	if (!itsSelection.IsEmpty())
		{
		const JIndex lineIndex = GetLineForChar(itsSelection.last);
		const JIndex charIndex = GetLineEnd(lineIndex);
		if (isspace(itsBuffer->GetCharacter(charIndex)))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
	else if (itsCaretLoc.charIndex == itsBuffer->GetLength()+1 &&
			 EndsWithNewline())
		{
		SetCaretLocation(itsCaretLoc);	// scroll to it
		}
	else if (!itsBuffer->IsEmpty())
		{
		const JIndex charIndex = GetLineEnd(itsCaretLoc.lineIndex);
		const JCharacter c     = itsBuffer->GetCharacter(charIndex);
		if (isspace(c) && (charIndex < itsBuffer->GetLength() || c == '\n'))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
}

/******************************************************************************
 CRLineIndexToVisualLineIndex

	Returns the index that the user sees that corresponds to the given
	index that would be seen without word wrap.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor::CRLineIndexToVisualLineIndex
	(
	const JIndex crLineIndex
	)
	const
{
	if (itsBreakCROnlyFlag)
		{
		return JMin(crLineIndex, GetLineCount());
		}
	else
		{
		JIndex charIndex = 0;
		for (JIndex i=1; i<=crLineIndex; i++)
			{
			const JIndex newCharIndex = GetParagraphEnd(charIndex+1);
			if (newCharIndex == charIndex)	// end of text
				{
				break;
				}
			charIndex = newCharIndex;
			}
		return GetLineForChar(GetParagraphStart(charIndex));
		}
}

/******************************************************************************
 VisualLineIndexToCRLineIndex

	Returns the index that would be seen without word wrap that corresponds
	to the given index that the user sees.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor::VisualLineIndexToCRLineIndex
	(
	const JIndex origVisualLineIndex
	)
	const
{
	const JIndex visualLineIndex = JMin(origVisualLineIndex, GetLineCount());
	if (itsBreakCROnlyFlag)
		{
		return visualLineIndex;
		}
	else
		{
		JSize crLineIndex         = 1;
		const JIndex endCharIndex = GetLineStart(visualLineIndex);
		for (JIndex i=1; i<endCharIndex; i++)
			{
			if (itsBuffer->GetCharacter(i) == '\n')
				{
				crLineIndex++;
				}
			}
		return crLineIndex;
		}
}

/******************************************************************************
 TEScrollTo (private)

	Scroll to the specified insertion point.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor::TEScrollTo
	(
	const CaretLocation& caretLoc
	)
{
	return TEScrollToRect(CalcCaretRect(caretLoc), kJFalse);
}

/******************************************************************************
 CalcCaretRect (private)

	Calculate the rectangle enclosing the caret when it is at the given
	location.  We set the width of the rectangle to be the left margin
	width so that the entire left margin will scroll into view when the
	caret is at the far left.

 ******************************************************************************/

JRect
JTextEditor::CalcCaretRect
	(
	const CaretLocation& caretLoc
	)
	const
{
	JRect r;

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 && EndsWithNewline())
		{
		r.top    = GetLineBottom(caretLoc.lineIndex) + 1;
		r.bottom = r.top + GetEWNHeight();
		r.left   = 0;
		r.right  = itsLeftMarginWidth + kRightMarginWidth;
		}
	else
		{
		r.top    = GetLineTop(caretLoc.lineIndex);
		r.bottom = r.top + GetLineHeight(caretLoc.lineIndex);
		r.left   = GetCharLeft(caretLoc);
		r.right  = r.left + itsLeftMarginWidth + kRightMarginWidth;
		}

	return r;
}

/******************************************************************************
 TEScrollToSelection (private)

	Scroll to make the selection or caret visible.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor::TEScrollToSelection
	(
	const JBoolean centerInDisplay
	)
{
	if (!itsSelection.IsEmpty())
		{
		const CaretLocation start = CalcCaretLocation(itsSelection.first);
		const CaretLocation end   = CalcCaretLocation(itsSelection.last);

		JRect r;
		r.top    = GetLineTop(start.lineIndex);
		r.bottom = GetLineBottom(end.lineIndex);
		if (start.lineIndex == end.lineIndex)
			{
			r.left  = GetCharLeft(start) + itsLeftMarginWidth;
			r.right = GetCharRight(end) + itsLeftMarginWidth;
			}
		else
			{
			r.left  = 0;
			r.right = itsLeftMarginWidth + itsWidth + kRightMarginWidth;
			}

		return TEScrollToRect(r, centerInDisplay);
		}
	else
		{
		return TEScrollToRect(CalcCaretRect(itsCaretLoc), centerInDisplay);
		}
}

/******************************************************************************
 TEUpdateDisplay (virtual protected)

	Called to update the window after mouse and keyboard events.
	Not pure virtual because some systems do this automatically.

 ******************************************************************************/

void
JTextEditor::TEUpdateDisplay()
{
}

/******************************************************************************
 TERefreshLines (protected)

	Redraw the specified range of lines.

 ******************************************************************************/

void
JTextEditor::TERefreshLines
	(
	const JIndex firstLine,
	const JIndex lastLine
	)
{
	JRect r(GetLineTop(firstLine), 0, GetLineBottom(lastLine)+1,
			itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) + kRightMarginWidth);
	if (lastLine == GetLineCount() && EndsWithNewline())
		{
		r.bottom += GetEWNHeight();
		}
	TERefreshRect(r);
}

/******************************************************************************
 TERefreshCaret (private)

 ******************************************************************************/

void
JTextEditor::TERefreshCaret
	(
	const CaretLocation& caretLoc
	)
{
	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())
		{
		const JRect r(itsHeight - GetEWNHeight(), 0, itsHeight,
					  itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) +
					  kRightMarginWidth);
		TERefreshRect(r);
		}
	else if (itsLineStarts->IndexValid(caretLoc.lineIndex))
		{
		TERefreshLines(caretLoc.lineIndex, caretLoc.lineIndex);
		}
}

/******************************************************************************
 MoveCaretVert (protected)

	Moves the caret up or down the specified number of lines.

 ******************************************************************************/

void
JTextEditor::MoveCaretVert
	(
	const JInteger deltaLines
	)
{
	assert( itsSelection.IsEmpty() );

	const JSize lineCount  = GetLineCount();
	const JIndex lineIndex = itsCaretLoc.lineIndex;

	JIndex newLineIndex = 0;
	if (deltaLines > 0 && lineIndex + deltaLines <= lineCount)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines > 0)
		{
		newLineIndex = lineCount;
		}
	else if (deltaLines < 0 && lineIndex > (JSize) -deltaLines)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines < 0)
		{
		newLineIndex = 1;
		}

	if (newLineIndex > 0)
		{
		const JCoordinate saveX = itsCaretX;

		const JPoint pt(itsCaretX, GetLineTop(newLineIndex)+1);
		SetCaretLocation(CalcCaretLocation(pt));

		itsCaretX = saveX;
		}
}

/******************************************************************************
 GetLineForChar

	Returns the line that contains the specified character.  Since the
	array is sorted, we can use a binary search.  We can use kAnyMatch
	because we know the values are unique.

 ******************************************************************************/

JIndex
JTextEditor::GetLineForChar
	(
	const JIndex charIndex
	)
	const
{
	if (charIndex == 0)
		{
		return 1;
		}

	itsLineStarts->SetCompareFunction(CompareCharacterIndices);

	JBoolean found;
	JIndex lineIndex =
		itsLineStarts->SearchSorted1(TextIndex(charIndex, 0), JListT::kAnyMatch, &found);
	if (!found)
		{
		lineIndex--;	// wants to insert -after- the value
		}
	return lineIndex;
}

/******************************************************************************
 GetLineForByte

	Returns the line that contains the specified byte.  Since the
	array is sorted, we can use a binary search.  We can use kAnyMatch
	because we know the values are unique.

 ******************************************************************************/

JIndex
JTextEditor::GetLineForByte
	(
	const JIndex byteIndex
	)
	const
{
	if (byteIndex == 0)
		{
		return 1;
		}

	// do not need to reset it, because both result in the same ordering
	itsLineStarts->SetCompareFunction(CompareByteIndices);

	JBoolean found;
	JIndex lineIndex =
		itsLineStarts->SearchSorted1(TextIndex(0, byteIndex), JListT::kAnyMatch, &found);
	if (!found)
		{
		lineIndex--;	// wants to insert -after- the value
		}
	return lineIndex;
}

/******************************************************************************
 CalcInsertionFont (private)

	Returns the font to use when inserting at the specified point.

 ******************************************************************************/

JFont
JTextEditor::CalcInsertionFont
	(
	const TextIndex& index
	)
	const
{
	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	return CalcInsertionFont(iter, *itsStyles);
}

JFont
JTextEditor::CalcInsertionFont
	(
	const JStringIterator&	buffer,
	const JRunArray<JFont>&	styles
	)
	const
{
	JUtf8Character c;
	if (buffer.Prev(&c, kJFalse) && c == '\n')
		{
		return styles.GetElement(buffer.GetNextCharacterIndex());
		}
	else if (charIndex > 1)
		{
		return styles.GetElement(buffer.GetPrevCharacterIndex());
		}
	else if (!styles.IsEmpty())
		{
		return styles.GetElement(1);
		}
	else
		{
		return itsDefFont;
		}
}

/******************************************************************************
 GetLineEnd

	Returns the last character on the specified line.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetLineEnd
	(
	const JIndex lineIndex
	)
	const
{
	if (lineIndex < GetLineCount())
		{
		return GetLineStart(lineIndex+1) - 1;
		}
	else
		{
		return itsPrevBufLength;	// consistent with rest of output from Recalc()
		}
}

/******************************************************************************
 GetLineTop

	Returns the top of the specified line.

 ******************************************************************************/

JCoordinate
JTextEditor::GetLineTop
	(
	const JIndex endLineIndex
	)
	const
{
	return (endLineIndex == 1 ? 0 :
			itsLineGeom->SumElements(1, endLineIndex-1, GetLineHeight));
}

/******************************************************************************
 GetCharLeft (private)

	Returns the starting x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

JCoordinate
JTextEditor::GetCharLeft
	(
	const CaretLocation& charLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(charLoc.lineIndex);

	JCoordinate x = 0;
	if (charLoc.charIndex > firstChar)
		{
		x = GetStringWidth(firstChar, charLoc.charIndex-1);
		}
	return x;
}

/******************************************************************************
 GetCharRight (private)

	Returns the ending x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetCharRight
	(
	const CaretLocation& charLoc
	)
	const
{
	return GetCharLeft(charLoc) + GetCharWidth(charLoc);
}

/******************************************************************************
 GetCharWidth (private)

	Returns the width of the specified character.

 ******************************************************************************/

JCoordinate
JTextEditor::GetCharWidth
	(
	const CaretLocation& charLoc
	)
	const
{
	const JCharacter c = itsBuffer->GetCharacter(charLoc.charIndex);
	if (c != '\t')
		{
		const JFont f = itsStyles->GetElement(charLoc.charIndex);
		return f.GetCharWidth(c);
		}
	else
		{
		return GetTabWidth(charLoc.charIndex, GetCharLeft(charLoc));
		}
}

/******************************************************************************
 GetStringWidth (private)

	Returns the width of the specified character range.

	The second version updates *runIndex,*firstInRun so that they
	are correct for endIndex+1.

 ******************************************************************************/

JCoordinate
JTextEditor::GetStringWidth
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
	const
{
	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(startIndex, &runIndex, &firstInRun);
	assert( found );

	return GetStringWidth(startIndex, endIndex, &runIndex, &firstInRun);
}

JCoordinate
JTextEditor::GetStringWidth
	(
	const JIndex	origStartIndex,
	const JIndex	endIndex,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	assert( IndexValid(origStartIndex) );
	assert( IndexValid(endIndex) );

	// preWidth stores the width of the characters preceding origStartIndex
	// on the line containing origStartIndex.  We calculate this -once- when
	// it is first needed.  (i.e. when we hit the first tab character)

	JCoordinate width    = 0;
	JCoordinate preWidth = -1;

	JIndex startIndex = origStartIndex;
	while (startIndex <= endIndex)
		{
		JSize runLength        = itsStyles->GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startIndex - *firstInRun;
		if (startIndex + runLength-1 > endIndex)
			{
			runLength = endIndex - startIndex + 1;
			}

		const JFont& f = itsStyles->GetRunDataRef(*runIndex);

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabIndex;
		if (LocateTab(startIndex, startIndex + runLength-1, &tabIndex))
			{
			runLength = tabIndex - startIndex;
			}
		else
			{
			tabIndex = 0;
			}

		if (runLength > 0)
			{
			width += f.GetStringWidth(itsBuffer->GetCString() + startIndex-1, runLength);
			}
		if (tabIndex > 0)
			{
			if (preWidth < 0)
				{
				// recursion: max depth 1
				preWidth = GetCharLeft(CalcCaretLocation(origStartIndex));
				assert( preWidth >= 0 );
				}
			width += GetTabWidth(startIndex + runLength, preWidth + width);
			runLength++;
			}

		startIndex += runLength;
		if (startIndex > trueRunEnd)	// move to next style run
			{
			*firstInRun = startIndex;
			(*runIndex)++;
			}
		}

	return width;
}

/******************************************************************************
 LocateTab (private)

	Returns the index of the first tab character, starting from startIndex.
	If no tab is found, returns kJFalse, and *tabIndex = 0.

 ******************************************************************************/

JBoolean
JTextEditor::LocateTab
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	JIndex*			tabIndex
	)
	const
{
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		if (itsBuffer->GetCharacter(i) == '\t')
			{
			*tabIndex = i;
			return kJTrue;
			}
		}

	*tabIndex = 0;
	return kJFalse;
}

/******************************************************************************
 GetTabWidth (virtual protected)

	Given the index of the tab character (charIndex) and the horizontal
	position on the line (in pixels) where the tab character starts (x),
	return the width of the tab character.

	This default implementation rounds the location up to the nearest
	multiple of itsDefTabWidth.

 ******************************************************************************/

JCoordinate
JTextEditor::GetTabWidth
	(
	const JIndex		charIndex,
	const JCoordinate	x
	)
	const
{
	const JCoordinate xnew = ((x / itsDefTabWidth) + 1) * itsDefTabWidth;
	return (xnew - x);
}

/******************************************************************************
 RecalcAll (protected)

 ******************************************************************************/

void
JTextEditor::RecalcAll
	(
	const JBoolean needAdjustStyles
	)
{
	if (itsBreakCROnlyFlag)
		{
		itsWidth = 0;
		}
	itsMaxWordWidth = 0;

	itsLineStarts->RemoveAll();
	itsLineStarts->AppendElement(TextIndex(1,1));

	itsLineGeom->RemoveAll();

	Recalc(CaretLocation(1,1,1), itsBuffer.GetCharacterCount(), kJFalse,
		   kJTrue, needAdjustStyles);
}

/******************************************************************************
 Recalc (private)

 ******************************************************************************/

inline void
JTextEditor::Recalc
	(
	const JIndex	startChar,
	const JSize		minCharCount,
	const JBoolean	deletion,
	const JBoolean	needCaretBcast,
	const JBoolean	needAdjustStyles
	)
{
	Recalc(CalcCaretLocation(startChar), minCharCount, deletion,
		   needCaretBcast, needAdjustStyles);
}

/******************************************************************************
 Recalc (private)

	Recalculate layout, starting from caretLoc and continuing at least
	through charCount characters.

	*** We assume that nothing above caretLoc.lineIndex needs to be recalculated.
		We also assume that the first line to recalculate already exists.

 ******************************************************************************/

void
JTextEditor::Recalc
	(
	const CaretLocation&	origCaretLoc,
	const JSize				origMinCharCount,
	const JBoolean			deletion,
	const JBoolean			needCaretBcast,
	const JBoolean			needAdjustStyles
	)
{
	JCoordinate maxLineWidth = 0;
	if (itsBreakCROnlyFlag && GetLineCount() == 1)
		{
		itsWidth = 0;
		}

	JCoordinate origY = -1;
	if (!itsLineGeom->IsEmpty())
		{
		origY = GetLineTop(origCaretLoc.lineIndex);
		}

	const JSize bufLength  = itsBuffer->GetLength();

	CaretLocation caretLoc = origCaretLoc;
	JSize minCharCount     = JMax((JSize) 1, origMinCharCount);
	JIndexRange redrawRange;
	if (bufLength > 0 && needAdjustStyles)
		{
		JIndexRange recalcRange(caretLoc.charIndex, caretLoc.charIndex + minCharCount - 1);
		redrawRange = recalcRange;
		AdjustStylesBeforeRecalc(*itsBuffer, itsStyles, &recalcRange, &redrawRange, deletion);
		caretLoc.charIndex = recalcRange.first;
		minCharCount       = recalcRange.GetLength();

		if (recalcRange.Contains(redrawRange))
			{
			redrawRange.SetToNothing();
			}

		assert( caretLoc.charIndex <= origCaretLoc.charIndex );
		assert( caretLoc.charIndex + minCharCount - 1 >=
				origCaretLoc.charIndex + origMinCharCount - 1 );
		assert( itsBuffer->GetLength() == itsStyles->GetElementCount() );

		if (caretLoc.charIndex < origCaretLoc.charIndex)
			{
			while (caretLoc.charIndex < itsLineStarts->GetElement(caretLoc.lineIndex))
				{
				(caretLoc.lineIndex)--;
				}
			}
		}

	if (caretLoc.charIndex == bufLength+1)
		{
		(caretLoc.charIndex)--;
		caretLoc.lineIndex = GetLineForChar(caretLoc.charIndex);
		}

	JIndex firstLineIndex, lastLineIndex;
	if (bufLength > 0)
		{
		Recalc1(bufLength, caretLoc, minCharCount, &maxLineWidth,
				&firstLineIndex, &lastLineIndex);

		if (!redrawRange.IsEmpty())
			{
			firstLineIndex = JMin(firstLineIndex, GetLineForChar(redrawRange.first));
			lastLineIndex  = JMax(lastLineIndex,  GetLineForChar(redrawRange.last));
			}
		}
	else
		{
		itsLineStarts->RemoveAll();
		itsLineStarts->AppendElement(1);

		itsLineGeom->RemoveAll();

		const JFont f = CalcInsertionFont(1);
		JCoordinate ascent,descent;
		const JCoordinate h = f.GetLineHeight(&ascent, &descent);
		itsLineGeom->AppendElement(LineGeometry(h, ascent));

		firstLineIndex = lastLineIndex = 1;
		}

	// If the caret is visible, recalculate the line index.

	JBoolean lineChanged = kJFalse;
	if (itsSelection.IsEmpty())
		{
		const JIndex origLine = itsCaretLoc.lineIndex;
		itsCaretLoc.lineIndex = GetLineForChar(itsCaretLoc.charIndex);
		lineChanged           = JI2B(itsCaretLoc.lineIndex != origLine);
		}

	// If we only break at newlines, we control our width.

	if (itsBreakCROnlyFlag && maxLineWidth > itsWidth)
		{
		itsWidth = maxLineWidth;
		}

	// If all the lines are the same height, set the scrolling step size to that.

	if (itsLineGeom->GetRunCount() == 1 && !itsIsPrintingFlag)
		{
		TESetVertScrollStep(GetLineHeight(1));
		}
	else if (!itsIsPrintingFlag)
		{
		TESetVertScrollStep(itsDefFont.GetLineHeight());
		}

	// recalculate the height

	const JSize lineCount = GetLineCount();
	JCoordinate newHeight = GetLineTop(lineCount) + GetLineHeight(lineCount);
	if (EndsWithNewline())
		{
		newHeight += GetEWNHeight();
		}

	JBoolean needRefreshLines = kJTrue;
	if (!itsIsPrintingFlag && newHeight != itsHeight)
		{
		TERefresh();	// redraw everything if the line heights changed
		needRefreshLines = kJFalse;
		}

	itsHeight = newHeight;

	// notify the derived class of our new size

	if (!itsIsPrintingFlag)
		{
		TESetGUIBounds(itsLeftMarginWidth + itsWidth + kRightMarginWidth, itsHeight, origY);
		}

	// save buffer length for next time

	itsPrevBufLength = bufLength;

	// show the changes

	if (!itsIsPrintingFlag && needRefreshLines)
		{
		TERefreshLines(firstLineIndex, lastLineIndex);
		}

	// This has to be last so everything is correctly set.

	if (needCaretBcast && itsSelection.IsEmpty())
		{
		BroadcastCaretMessages(itsCaretLoc, lineChanged);
		}
	else if (needCaretBcast)
		{
		BroadcastCaretMessages(CalcCaretLocation(itsSelection.first), kJTrue);
		}

	// avoid ludicrous width

	if (TEWidthIsBeyondDisplayCapacity(itsWidth))
		{
		SetBreakCROnly(kJFalse);
		}
}

/******************************************************************************
 Recalc1 (private)

	Subroutine called by Recalc().

	(firstLineIndex, lastLineIndex) gives the range of lines that had
	to be recalculated.

 ******************************************************************************/

void
JTextEditor::Recalc1
	(
	const JSize				bufLength,
	const CaretLocation&	caretLoc,
	const JSize				origMinCharCount,
	JCoordinate*			maxLineWidth,
	JIndex*					firstLineIndex,
	JIndex*					lastLineIndex
	)
{
	JIndex lineIndex = caretLoc.lineIndex;
	if (!itsBreakCROnlyFlag && lineIndex > 1 &&
		caretLoc.charIndex <= bufLength &&
		NoPrevWhitespaceOnLine(*itsBuffer, caretLoc))
		{
		// If we start in the first word on the line, it
		// might actually belong on the previous line.

		lineIndex--;
		}

	JIndex firstChar   = GetLineStart(lineIndex);
	JSize minCharCount = origMinCharCount + (caretLoc.charIndex - firstChar);
	*firstLineIndex    = lineIndex;

	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(firstChar, &runIndex, &firstInRun);
	assert( found );

	JSize totalCharCount = 0;
	*maxLineWidth        = itsWidth;
	while (1)
		{
		JCoordinate lineWidth;
		const JSize charCount = RecalcLine(bufLength, firstChar, lineIndex, &lineWidth,
										   &runIndex, &firstInRun);
		totalCharCount += charCount;
		if (*maxLineWidth < lineWidth)
			{
			*maxLineWidth = lineWidth;
			}
		const JIndex endChar = firstChar + charCount-1;
		assert( endChar <= bufLength );

		// remove line starts that are further from the end than the new one
		// (we use (bufLength - endChar) so subtraction won't produce negative numbers)

		while (lineIndex < GetLineCount() &&
			   (itsPrevBufLength+1) - GetLineStart(lineIndex+1) >
					bufLength - endChar)
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}

		// check if we are done

		if (endChar >= bufLength)
			{
			// We reached the end of the text.

			break;
			}
		else if (totalCharCount >= minCharCount &&
				 lineIndex < GetLineCount() &&
				 itsPrevBufLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			// The rest of the line starts merely shift.

			const JSize lineCount = itsLineStarts->GetElementCount();
			assert( lineCount > lineIndex );
			const long delta = endChar+1 - GetLineStart(lineIndex+1);
			if (delta != 0)
				{
				const JIndex* lineStart = itsLineStarts->GetCArray();
				for (JIndex i=lineIndex+1; i<=lineCount; i++)
					{
					itsLineStarts->SetElement(i, lineStart[i-1] + delta);
					}
				}
			break;
			}

		// insert the new line start

		lineIndex++;
		firstChar += charCount;

		itsLineStarts->InsertElementAtIndex(lineIndex, firstChar);
		itsLineGeom->InsertElementAtIndex(lineIndex, LineGeometry());

		// This catches the case when the new and old line starts
		// are equally far from the end, but we haven't recalculated
		// far enough yet, so the above breakout code didn't trigger.

		if (lineIndex < GetLineCount() &&
			   itsPrevBufLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}
		}

	*lastLineIndex = lineIndex;
}

/******************************************************************************
 RecalcLine (private)

	Recalculate the line starting with firstChar.  Returns the number
	of characters on the line.  Sets the appropriate values in itsLineGeom.
	Sets *lineWidth to the width of the line in pixels.

	If insertLine is kJTrue, then this line is new, so we insert a new
	element into itsLineGeom.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

JSize
JTextEditor::RecalcLine
	(
	const JSize		bufLength,
	const JIndex	firstCharIndex,
	const JIndex	lineIndex,
	JCoordinate*	lineWidth,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
{
	JSize charCount = 0;
	*lineWidth      = 0;

	JIndex gswRunIndex   = *runIndex;
	JIndex gswFirstInRun = *firstInRun;

	if (itsBreakCROnlyFlag)
		{
		JIndex endIndex = firstCharIndex;
		if (!itsBuffer->LocateNextSubstring("\n", &endIndex))
			{
			endIndex = itsBuffer->GetLength();
			}
		charCount  = endIndex - firstCharIndex + 1;
		*lineWidth = GetStringWidth(firstCharIndex, endIndex,
									&gswRunIndex, &gswFirstInRun);
		}

	else
		{
		// include leading whitespace

		JBoolean endOfLine;
		charCount = IncludeWhitespaceOnLine(bufLength, firstCharIndex,
											lineWidth, &endOfLine,
											&gswRunIndex, &gswFirstInRun);
		JIndex charIndex = firstCharIndex + charCount;

		// Add words until we hit the right margin, a newline,
		// or the end of the buffer.

		while (charIndex <= bufLength && !endOfLine)
			{
			// get the next word

			JIndex prevIndex = charIndex;
			if (!LocateNextWhitespace(bufLength, &charIndex))
				{
				charIndex = bufLength+1;
				}

			// check if the word fits on this line

			JCoordinate dw =
				GetStringWidth(prevIndex, charIndex-1, &gswRunIndex, &gswFirstInRun);
			if (itsMaxWordWidth < dw)
				{
				itsMaxWordWidth = dw;
				}
			if (*lineWidth + dw > itsWidth)
				{
				if (prevIndex != firstCharIndex)
					{
					// this word goes on the next line

					charIndex = prevIndex;
					}
				else
					{
					// put as much of this word as possible on the line

					assert( *lineWidth == 0 && charCount == 0 );
					charCount = GetSubwordForLine(bufLength, lineIndex,
												  firstCharIndex, lineWidth);
					}
				break;
				}

			// put the word on this line

			*lineWidth += dw;
			charCount  += charIndex - prevIndex;

			// include the whitespace after the word

			JSize wsCount =
				IncludeWhitespaceOnLine(bufLength, charIndex, lineWidth, &endOfLine,
										&gswRunIndex, &gswFirstInRun);
			charIndex += wsCount;
			charCount += wsCount;
			}
		}

	// update geometry for this line

	const JSize runCount = itsStyles->GetRunCount();
	const JSize lastChar = firstCharIndex + charCount-1;

	JCoordinate maxAscent=0, maxDescent=0;
	while (*runIndex <= runCount)
		{
		const JFont& f = itsStyles->GetRunDataRef(*runIndex);
		JCoordinate ascent, descent;
		f.GetLineHeight(&ascent, &descent);

		if (ascent > maxAscent)
			{
			maxAscent = ascent;
			}
		if (descent > maxDescent)
			{
			maxDescent = descent;
			}

		const JIndex firstInNextRun = *firstInRun + itsStyles->GetRunLength(*runIndex);
		if (firstInNextRun <= lastChar+1)
			{
			(*runIndex)++;
			*firstInRun = firstInNextRun;
			}
		if (firstInNextRun > lastChar)
			{
			break;
			}
		}

	const LineGeometry geom(maxAscent+maxDescent, maxAscent);
	if (lineIndex <= itsLineGeom->GetElementCount())
		{
		itsLineGeom->SetElement(lineIndex, geom);
		}
	else
		{
		itsLineGeom->AppendElement(geom);
		}

	// return number of characters on line

	return charCount;
}

/******************************************************************************
 IncludeWhitespaceOnLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of consecutive whitespace
	characters encountered.  Increments *lineWidth with the width of this
	whitespace.  If we encounter a newline, we stop beyond it and set
	*endOfLine to kJTrue.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the whitespace.

 ******************************************************************************/

JSize
JTextEditor::IncludeWhitespaceOnLine
	(
	const JSize		bufLength,
	const JIndex	origStartIndex,
	JCoordinate*	lineWidth,
	JBoolean*		endOfLine,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	*endOfLine = kJFalse;

	JIndex startIndex = origStartIndex;
	JIndex endIndex   = startIndex;
	JSize wsCount     = 0;

	while (endIndex <= bufLength)
		{
		const JCharacter c = itsBuffer->GetCharacter(endIndex);
		if (!isspace(c))
			{
			break;
			}

		wsCount++;
		if (c == '\t')
			{
			if (endIndex > startIndex)
				{
				*lineWidth += GetStringWidth(startIndex, endIndex-1,
											 runIndex, firstInRun);
				}
			*lineWidth += GetTabWidth(endIndex, *lineWidth);
			startIndex  = endIndex+1;

			// update *runIndex,*firstInRun after passing tab character

			const JSize runLength = itsStyles->GetRunLength(*runIndex);
			if (startIndex > *firstInRun + runLength-1)
				{
				*firstInRun += runLength;
				(*runIndex)++;
				}

			// tab characters can wrap to the next line

			if (!itsBreakCROnlyFlag && *lineWidth > itsWidth)
				{
				*endOfLine = kJTrue;
				break;
				}
			}

		endIndex++;
		if (c == '\n')
			{
			*endOfLine = kJTrue;
			break;
			}
		}

	if (endIndex > startIndex)
		{
		*lineWidth += GetStringWidth(startIndex, endIndex-1, runIndex, firstInRun);
		}

	return wsCount;
}

/******************************************************************************
 LocateNextWhitespace (private)

	*** Only for use by RecalcLine()

	Find the next whitespace character, starting from the given index.
	Returns kJFalse if it doesn't find any.

	*** If we only break at newlines, only newlines are considered to be
		whitespace.

 ******************************************************************************/

JBoolean
JTextEditor::LocateNextWhitespace
	(
	const JSize	bufLength,
	JIndex*		startIndex
	)
	const
{
	for (JIndex i=*startIndex; i<=bufLength; i++)
		{
		const JCharacter c = itsBuffer->GetCharacter(i);
		if ((!itsBreakCROnlyFlag && isspace(c)) ||
			( itsBreakCROnlyFlag && c == '\n'))
			{
			*startIndex = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetSubwordForLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of characters that
	will fit on a line.  Sets *lineWidth to the width of the line in pixels.
	We always put at least one character on the line.

 ******************************************************************************/

JSize
JTextEditor::GetSubwordForLine
	(
	const JSize		bufLength,
	const JIndex	lineIndex,
	const JIndex	startIndex,
	JCoordinate*	lineWidth
	)
	const
{
	*lineWidth = 0;

	JIndex endIndex = 0;
	CaretLocation caretLoc(0, lineIndex);
	for (JIndex i=startIndex; i<=bufLength; i++)
		{
		caretLoc.charIndex   = i;
		const JCoordinate dw = GetCharWidth(caretLoc);
		if (i > startIndex && *lineWidth + dw > itsWidth)
			{
			endIndex = i-1;
			break;
			}
		else
			{
			*lineWidth += dw;
			}
		}
	if (endIndex == 0)
		{
		endIndex = bufLength;
		}

	return (endIndex - startIndex + 1);
}

/******************************************************************************
 NoPrevWhitespaceOnLine (private)

	Returns kJTrue if there is no whitespace between startIndex-1 and
	the first character on the line.

	Called by Recalc1() to decide whether or not to start on the
	previous line.

 ******************************************************************************/

JBoolean
JTextEditor::NoPrevWhitespaceOnLine
	(
	const JCharacter*		str,
	const CaretLocation&	startLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(startLoc.lineIndex);
	for (JIndex i = startLoc.charIndex - 1; i>firstChar; i--)
		{
		if (isspace(str[i-1]))
			{
			return kJFalse;
			}
		}

	return kJTrue;	// we hit the start of the string
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Called before Recalc() begins its work.  Derived classes can override
	this to adjust the style of the affected range of text.  range is an
	in/out variable because the changes usually slop out beyond the initial
	range.

	*** The endpoints of the ranges are only allowed to move outward.

	*** The contents of *styles can change, but the length must remain
		the same.  *range must be expanded to include all the changed
		elements.

 ******************************************************************************/

void
JTextEditor::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
}

/******************************************************************************
 CalcCaretLocation (protected)

	Return the closest insertion point.  If the line ends with a space,
	the last possible insertion point is in front of this space

 ******************************************************************************/

JTextEditor::CaretLocation
JTextEditor::CalcCaretLocation
	(
	const JPoint& pt
	)
	const
{
	const JSize bufLength = itsBuffer->GetLength();
	if (bufLength == 0)
		{
		return CaretLocation(1,1);
		}
	else if (pt.y >= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the line that was clicked on

	JCoordinate lineTop;
	const JIndex lineIndex = CalcLineIndex(pt.y, &lineTop);
	if (EndsWithNewline() &&
		itsHeight - GetEWNHeight() < pt.y && pt.y <= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the closest insertion point

	JIndex charIndex = 0;
	{
	const JIndex lineStart = GetLineStart(lineIndex);

	JIndex lineEnd = GetLineEnd(lineIndex);
	if ((lineEnd < bufLength || EndsWithNewline()) &&
		isspace(itsBuffer->GetCharacter(lineEnd)))
		{
		lineEnd--;
		}

	JCoordinate prevD = pt.x;
	if (prevD <= 0)
		{
		charIndex = lineStart;
		}
	else
		{
		JCoordinate x = 0;
		for (JIndex i=lineStart; i<=lineEnd; i++)
			{
			x += GetCharWidth(CaretLocation(i, lineIndex));
			const JCoordinate d = pt.x - x;
			if (d == 0)
				{
				charIndex = i+1;
				break;
				}
			else if (d < 0 && prevD <= -d)
				{
				charIndex = i;
				break;
				}
			else if (d < 0 && prevD > -d)
				{
				charIndex = i+1;
				break;
				}
			prevD = d;
			}

		if (charIndex == 0)
			{
			charIndex = lineEnd+1;
			}
		}
	}

	return CaretLocation(charIndex, lineIndex);
}

/******************************************************************************
 CalcCaretLocation (private)

	byteIndex can be zero.

 ******************************************************************************/

inline JTextEditor::CaretLocation
JTextEditor::CalcCaretLocation
	(
	const TextIndex& index
	)
	const
{
	const JIndex lineIndex = GetLineForChar(index.charIndex);

	JIndex byteIndex = index.byteIndex;
	if (byteIndex == 0)
		{
		const TextIndex& i = itsLineStarts->GetElement(lineIndex);

		byteIndex += JString::CountBytes(itsBuffer.GetBytes() + i.byteIndex - 1,
										 charIndex - i.charIndex);
		}

	return CaretLocation(index.charIndex, byteIndex, lineIndex);
}

/******************************************************************************
 CalcLineIndex (private)

 ******************************************************************************/

JIndex
JTextEditor::CalcLineIndex
	(
	const JCoordinate	y,
	JCoordinate*		lineTop
	)
	const
{
	if (y < 0)
		{
		*lineTop = 0;
		return 1;
		}
	else
		{
		JIndex lineIndex;
		itsLineGeom->FindPositiveSum(y, 1, &lineIndex, lineTop, GetLineHeight);
		return lineIndex;
		}
}

/******************************************************************************
 GetLineHeight (static private)

	Used by GetLineTop() and CalcLineIndex().

 ******************************************************************************/

JInteger
JTextEditor::GetLineHeight
	(
	const LineGeometry& data
	)
{
	return data.height;
}

/******************************************************************************
 GetEWNHeight (private)

	Not inline to avoid including JFontManager.h in header file

 ******************************************************************************/

JCoordinate
JTextEditor::GetEWNHeight()
	const
{
	const JFont f = CalcInsertionFont(
						TextIndex(itsBuffer.GetCharacterCount() + 1,
								  itsBuffer.GetByteCount() + 1));
	return f.GetLineHeight();
}

/******************************************************************************
 CompareCharacterIndices (static private)

 ******************************************************************************/

JListT::CompareResult
JTextEditor::CompareCharacterIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return JCompareIndices(i.charIndex, j.charIndex);
}

/******************************************************************************
 CompareByteIndices (static private)

 ******************************************************************************/

JListT::CompareResult
JTextEditor::CompareByteIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return JCompareIndices(i.byteIndex, j.byteIndex);
}
