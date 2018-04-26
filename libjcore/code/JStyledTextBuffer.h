/******************************************************************************
 JStyledTextBuffer.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStyledTextBuffer
#define _H_JStyledTextBuffer

#include <JBroadcaster.h>
#include <JFont.h>
#include <JRunArray.h>
#include <JPtrArray-JString.h>
#include <JStringMatch.h>

class JRegex;
class JInterpolate;
class JFontManager;
class JColormap;
class JTEUndoBase;
class JTEUndoTextBase;
class JTEUndoTyping;
class JTEUndoStyle;
class JTEUndoPaste;
class JTEUndoTabShift;
class JTEUndoMove;

typedef JBoolean (*JCharacterInWordFn)(const JUtf8Character&);

class JStyledTextBuffer : virtual public JBroadcaster
{
	friend class JTEUndoTextBase;
	friend class JTEUndoStyle;

public:

	enum PlainTextFormat
	{
		kUNIXText,
		kMacintoshText,
		kDOSText
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

		JStyledTextBuffer::TextCount& operator+=(const JStyledTextBuffer::TextCount& c);
	};

	struct TextRange
	{
		JCharacterRange	charRange;
		JUtf8ByteRange	byteRange;

		TextRange()
		{ };

		TextRange(const JCharacterRange& cr, const JUtf8ByteRange& br)
			:
			charRange(cr),
			byteRange(br)
		{ };

		TextRange(const JStringMatch& m)
			:
			charRange(m.GetCharacterRange()),
			byteRange(m.GetUtf8ByteRange())
		{ };

		TextRange(const TextIndex& i, const TextCount& c)
		{
			charRange.SetFirstAndCount(i.charIndex, c.charCount);
			byteRange.SetFirstAndCount(i.byteIndex, c.byteCount);
		};

		JBoolean
		IsEmpty() const
		{
			return charRange.IsEmpty();
		};

		void
		SetToNothing()
		{
			charRange.SetToNothing();
			byteRange.SetToNothing();
		};

		TextCount
		GetCount() const
		{
			return TextCount(charRange.GetCount(), byteRange.GetCount());
		};

		void
		SetCount(const TextCount& c)
		{
			charRange.SetFirstAndCount(charRange.first, c.charCount);
			byteRange.SetFirstAndCount(byteRange.first, c.byteCount);
		};
	};

	struct CRMRule
	{
		friend JStyledTextBuffer;

		CRMRule()
			:
			first(NULL), rest(NULL), replace(NULL)
		{ };

		CRMRule(JRegex* f, JRegex* r, JString* repl)
			:
			first(f), rest(r), replace(repl)
		{ };

		CRMRule(const JString& firstPattern, const JString& restPattern,
				const JString& replacePattern);

		private:

		JRegex*		first;
		JRegex*		rest;
		JString*	replace;
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

	JStyledTextBuffer(const JFontManager* fontManager, JColormap* colormap,
					  const JBoolean pasteStyledText);
	JStyledTextBuffer(const JStyledTextBuffer& source);

	virtual ~JStyledTextBuffer();

	JBoolean	WillPasteStyledText() const;
	void		ShouldPasteStyledText(const JBoolean pasteStyled);

	JBoolean				IsEmpty() const;
	JBoolean				IsEntireWord() const;
	JBoolean				EndsWithNewline() const;
	const JString&			GetText() const;
	const JRunArray<JFont>&	GetStyles() const;
	JBoolean				SetText(const JString& text,
									const JRunArray<JFont>* style = NULL);

	JBoolean	IsCharacterInWord(const JUtf8Character& c) const;

	TextIndex	GetWordStart(const TextIndex& index) const;
	TextIndex	GetWordEnd(const TextIndex& index) const;
	TextIndex	GetPartialWordStart(const TextIndex& index) const;
	TextIndex	GetPartialWordEnd(const TextIndex& index) const;
	TextIndex	GetParagraphStart(const TextIndex& index) const;
	TextIndex	GetParagraphEnd(const TextIndex& index) const;
	TextIndex	AdjustTextIndex(const TextIndex& index, const JInteger charDelta) const;

	JIndex		GetColumnForChar(const TextIndex& lineStart, const TextIndex& location) const;

	JBoolean	ReadPlainText(const JString& fileName, PlainTextFormat* format,
							  const JBoolean acceptBinaryFile = kJTrue);
	void		WritePlainText(const JString& fileName, const PlainTextFormat format) const;
	void		WritePlainText(std::ostream& output, const PlainTextFormat format) const;

	JBoolean	ReadPrivateFormat(std::istream& input);
	void		WritePrivateFormat(std::ostream& output) const;

	static JBoolean	ReadPrivateFormat(std::istream& input, const JStyledTextBuffer* te,
									  JString* text, JRunArray<JFont>* style);
	static void	WritePrivateFormat(std::ostream& output,
								   const JColormap* colormap, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange);

	JStringMatch	SearchForward(const TextIndex& startIndex,
								  const JRegex& regex, const JBoolean entireWord,
								  const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SearchBackward(const TextIndex& startIndex,
								   const JRegex& regex, const JBoolean entireWord,
								   const JBoolean wrapSearch, JBoolean* wrapped);

	void		ReplaceRange(JStringIterator* buffer, JRunArray<JFont>* styles,
							 const JStringMatch& match,
							 const JString& replaceStr,
							 const JBoolean replaceIsRegex,
							 JInterpolate& interpolator,
							 const JBoolean preserveCase);
	JBoolean	IsEntireWord(const JString& buffer, const TextRange& range) const;

	JBoolean	SearchForward(const FontMatch& match, const TextIndex& startIndex,
							  const JBoolean wrapSearch,
							  JBoolean* wrapped, TextRange* range);
	JBoolean	SearchBackward(const FontMatch& match, const TextIndex& startIndex,
							   const JBoolean wrapSearch,
							   JBoolean* wrapped, TextRange* range);

	const JFontManager*	GetFontManager() const;
	JColormap*			GetColormap() const;

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
	void		SetFont(const JIndex startIndex, const JRunArray<JFont>& f,
						const JBoolean clearUndo);

	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const JBoolean clearUndo);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name);
	void	SetDefaultFontSize(const JSize size);
	void	SetDefaultFontStyle(const JFontStyle& style);
	void	SetDefaultFont(const JFont& f);

	JFont	CalcInsertionFont(const TextIndex& index) const;

	void		Outdent(const TextRange& range, const JSize tabCount = 1,
						const JBoolean force = kJFalse);
	void		Indent(const TextRange& range, const JSize tabCount = 1);

	TextRange	CleanWhitespace(const TextRange& range, const JBoolean align);

	JBoolean	TabInsertsSpaces() const;
	void		TabShouldInsertSpaces(const JBoolean spaces);

	JBoolean	WillShowWhitespace() const;
	void		ShouldShowWhitespace(const JBoolean show);

	JBoolean	WillMoveToFrontOfText() const;
	void		ShouldMoveToFrontOfText(const JBoolean moveToFront);

	void		Undo();
	void		Redo();
	void		ClearUndo();
	void		DeactivateCurrentUndo();

	JBoolean	IsUsingMultipleUndo() const;
	void		UseMultipleUndo(const JBoolean useMultiple = kJTrue);

	JSize		GetUndoDepth() const;
	void		SetUndoDepth(const JSize maxUndoCount);

	JBoolean	IsAtLastSaveLocation() const;
	void		SetLastSaveLocation();
	void		ClearLastSaveLocation();

	JBoolean	Copy(const TextRange& range,
					 JString* text, JRunArray<JFont>* style = NULL) const;
	void		Paste(const TextRange& range,
					  const JString& text, const JRunArray<JFont>* style = NULL);

	void	MoveText(const TextRange& srcRange, const TextIndex& origDestIndex,
					 const JBoolean copy);

	TextIndex	BackwardDelete(const TextIndex&	lineStart, const TextIndex&	caretIndex,
							   const JBoolean deleteToTabStop,
							   JString* returnText = NULL, JRunArray<JFont>* returnStyle = NULL);
	void		ForwardDelete(const TextIndex&	lineStart, const TextIndex&	caretIndex,
							  const JBoolean deleteToTabStop,
							  JString* returnText = NULL, JRunArray<JFont>* returnStyle = NULL);

	static JBoolean	ContainsIllegalChars(const JString& text);
	static JBoolean	RemoveIllegalChars(JString* text, JRunArray<JFont>* style = NULL);

	JBoolean	CleanRightMargin(const JBoolean coerce, JCharacterRange* reformatRange);

	JSize		GetCRMLineWidth() const;
	void		SetCRMLineWidth(const JSize charCount);

	JSize		GetCRMTabCharCount() const;
	void		SetCRMTabCharCount(const JSize charCount);
	JSize		CRMGetTabWidth(const JIndex textColumn) const;

	JBoolean	GetCRMRuleList(const CRMRuleList** ruleList) const;
	void		SetCRMRuleList(CRMRuleList* ruleList,
							   const JBoolean teOwnsRuleList);
	void		ClearCRMRuleList();

	JCharacterInWordFn	GetCharacterInWordFunction() const;
	void				SetCharacterInWordFunction(JCharacterInWordFn f);

	JBoolean	WillBroadcastAllTextChanged() const;
	void		ShouldBroadcastAllTextChanged(const JBoolean broadcast);

	JBoolean	CharacterIndexValid(const JIndex charIndex) const;

	static JListT::CompareResult
		CompareCharacterIndices(const TextIndex& i, const TextIndex& j);
	static JListT::CompareResult
		CompareByteIndices(const TextIndex& i, const TextIndex& j);

protected:

	JFont	CalcInsertionFont(JStringIterator& buffer,
							  const JRunArray<JFont>& styles) const;

	JBoolean			GetCurrentUndo(JTEUndoBase** undo) const;
	JBoolean			GetCurrentRedo(JTEUndoBase** redo) const;
	void				NewUndo(JTEUndoBase* undo, const JBoolean isNew);
	void				ReplaceUndo(JTEUndoBase* oldUndo, JTEUndoBase* newUndo);
	void				ClearOutdatedUndo();
	JTEUndoTyping*		GetTypingUndo(const JStyledTextBuffer::TextIndex& start, JBoolean* isNew);
	JTEUndoStyle*		GetStyleUndo(const JCharacterRange& range, JBoolean* isNew);
	JTEUndoPaste*		GetPasteUndo(const JStyledTextBuffer::TextRange& range, JBoolean* isNew);
	JTEUndoTabShift*	GetTabShiftUndo(const JStyledTextBuffer::TextRange& range, JBoolean* isNew);
	JTEUndoMove*		GetMoveUndo(const JStyledTextBuffer::TextIndex& srcIndex,
									const JStyledTextBuffer::TextIndex& destIndex,
									const JStyledTextBuffer::TextCount& count,
									JBoolean* isNew);

	TextCount	PrivatePaste(const TextRange& range,
							 const JString& text, const JRunArray<JFont>* style);
	TextCount	InsertText(const TextIndex& index, const JString& text,
						   const JRunArray<JFont>* style = NULL,
						   const JFont* defaultStyle = NULL);
	TextCount	InsertText(JStringIterator* targetText, JRunArray<JFont>* targetStyle,
						   const JString& text, const JRunArray<JFont>* style,
						   const JFont* defaultStyle);
	void		DeleteText(const TextRange& range);
	void		DeleteText(JStringIterator* iter, const JSize charCount);
	JBoolean	CleanText(const JString& text, const JRunArray<JFont>* style,
						  JString** cleanText, JRunArray<JFont>** cleanStyle,
						  JBoolean* okToInsert);

	JUtf8ByteRange	CharToByteRange(const TextIndex* lineStart, const JCharacterRange& charRange) const;

	static JUtf8ByteRange	CharToByteRange(const JCharacterRange& charRange,
											JStringIterator* iter);

	void	AutoIndent(JTEUndoTyping* typingUndo);
	void	InsertSpacesForTab(const TextIndex& lineStart, const TextIndex& caretIndex);

	virtual JBoolean	NeedsToFilterText(const JString& text) const;
	virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style);

private:

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

private:

	JString				itsBuffer;
	JRunArray<JFont>*	itsStyles;
	JBoolean			itsPasteStyledTextFlag;		// kJTrue => paste styled text
	JBoolean			itsTabToSpacesFlag;			// kJTrue => 1 tab -> itsCRMTabCharCount spaces
	JBoolean			itsBcastAllTextChangedFlag;	// kJTrue => broadcast TextChanged every time

	const JFontManager*	itsFontMgr;
	JFont				itsDefFont;

	JColormap*			itsColormap;			// not owned

	JTEUndoBase*			itsUndo;				// can be NULL
	JPtrArray<JTEUndoBase>*	itsUndoList;			// NULL if not multiple undo
	JIndex					itsFirstRedoIndex;		// range [1:count+1]
	JInteger				itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState				itsUndoState;
	JSize					itsMaxUndoCount;		// maximum length of itsUndoList

	JCharacterInWordFn	itsCharInWordFn;

	JSize	itsCRMLineWidth;
	JSize	itsCRMTabCharCount;

	CRMRuleList*	itsCRMRuleList;		// can be NULL
	JBoolean		itsOwnsCRMRulesFlag;

private:

	static JBoolean	DefaultIsCharacterInWord(const JUtf8Character& c);

	static void	WritePrivateFormat(std::ostream& output,
								   const JColormap* colormap, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange,
								   const JUtf8ByteRange& byteRange);

	static void	ConvertFromMacintoshNewlinetoUNIXNewline(JString* buffer);

	// not allowed

	const JStyledTextBuffer& operator=(const JStyledTextBuffer& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTextSet;
	static const JUtf8Byte* kTextChanged;
	static const JUtf8Byte* kWillBeBusy;

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

	class WillBeBusy : public JBroadcaster::Message
		{
		public:

			WillBeBusy()
				:
				JBroadcaster::Message(kWillBeBusy)
				{ };
		};
};


/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::IsEmpty()
	const
{
	return itsBuffer.IsEmpty();
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the text is a single, complete word.

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::IsEntireWord()
	const
{
	return IsEntireWord(itsBuffer, TextRange(
						JCharacterRange(1, itsBuffer.GetCharacterCount()),
						JUtf8ByteRange(1, itsBuffer.GetByteCount())));
}

/******************************************************************************
 GetText

 ******************************************************************************/

inline const JString&
JStyledTextBuffer::GetText()
	const
{
	return itsBuffer;
}

/******************************************************************************
 CharacterIndexValid

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::CharacterIndexValid
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
JStyledTextBuffer::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 Multiple undo

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::IsUsingMultipleUndo()
	const
{
	return JI2B( itsUndoList != NULL );
}

inline JSize
JStyledTextBuffer::GetUndoDepth()
	const
{
	return itsMaxUndoCount;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::IsAtLastSaveLocation()
	const
{
	return JI2B( itsUndoList != NULL &&
				 itsLastSaveRedoIndex > 0 &&
				 JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex );
}

inline void
JStyledTextBuffer::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
JStyledTextBuffer::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

/******************************************************************************
 GetFontManager

 ******************************************************************************/

inline const JFontManager*
JStyledTextBuffer::GetFontManager()
	const
{
	return itsFontMgr;
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JColormap*
JStyledTextBuffer::GetColormap()
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
JStyledTextBuffer::WillPasteStyledText()
	const
{
	return itsPasteStyledTextFlag;
}

inline void
JStyledTextBuffer::ShouldPasteStyledText
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
JStyledTextBuffer::GetDefaultFont()
	const
{
	return itsDefFont;
}

/******************************************************************************
 Tab inserts spaces

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::TabInsertsSpaces()
	const
{
	return itsTabToSpacesFlag;
}

inline void
JStyledTextBuffer::TabShouldInsertSpaces
	(
	const JBoolean spaces
	)
{
	itsTabToSpacesFlag = spaces;
}

/******************************************************************************
 CleanRightMargin() parameters

 ******************************************************************************/

inline JSize
JStyledTextBuffer::GetCRMLineWidth()
	const
{
	return itsCRMLineWidth;
}

inline JSize
JStyledTextBuffer::GetCRMTabCharCount()
	const
{
	return itsCRMTabCharCount;
}

inline JBoolean
JStyledTextBuffer::GetCRMRuleList
	(
	const CRMRuleList** ruleList
	)
	const
{
	*ruleList = itsCRMRuleList;
	return JI2B( itsCRMRuleList != NULL );
}

/******************************************************************************
 Always broadcast TextChanged

	If this is set, TextChanged is always broadcast instead of optimizing
	out redundant messages generated by the active undo object.

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::WillBroadcastAllTextChanged()
	const
{
	return itsBcastAllTextChangedFlag;
}

inline void
JStyledTextBuffer::ShouldBroadcastAllTextChanged
	(
	const JBoolean broadcast
	)
{
	itsBcastAllTextChangedFlag = broadcast;
}

/******************************************************************************
 Font access

 ******************************************************************************/

inline JFont
JStyledTextBuffer::GetFont
	(
	const JIndex charIndex
	)
	const
{
	return itsStyles->GetElement(charIndex);
}

inline void
JStyledTextBuffer::SetDefaultFontName
	(
	const JString& name
	)
{
	itsDefFont.SetName(name);
}

inline void
JStyledTextBuffer::SetDefaultFontSize
	(
	const JSize size
	)
{
	itsDefFont.SetSize(size);
}

inline void
JStyledTextBuffer::SetDefaultFontStyle
	(
	const JFontStyle& style
	)
{
	itsDefFont.SetStyle(style);
}

inline void
JStyledTextBuffer::SetDefaultFont
	(
	const JFont& font
	)
{
	itsDefFont = font;
}

/******************************************************************************
 EndsWithNewline

 ******************************************************************************/

inline JBoolean
JStyledTextBuffer::EndsWithNewline()
	const
{
	return JI2B(!itsBuffer.IsEmpty() &&
				itsBuffer.GetLastCharacter() == '\n' );
}

/******************************************************************************
 GetCharacterInWordFunction

 ******************************************************************************/

inline JCharacterInWordFn
JStyledTextBuffer::GetCharacterInWordFunction()
	const
{
	return itsCharInWordFn;
}

/******************************************************************************
 TextCount operators

 ******************************************************************************/

inline JStyledTextBuffer::TextCount
operator+
	(
	const JStyledTextBuffer::TextCount& c1,
	const JStyledTextBuffer::TextCount& c2
	)
{
	return JStyledTextBuffer::TextCount(c1.charCount + c2.charCount, c1.byteCount + c2.byteCount);
}

inline JStyledTextBuffer::TextCount&
JStyledTextBuffer::TextCount::operator+=
	(
	const JStyledTextBuffer::TextCount& c
	)
{
	charCount += c.charCount;
	byteCount += c.byteCount;
	return *this;
}

#endif
