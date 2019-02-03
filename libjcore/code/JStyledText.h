/******************************************************************************
 JStyledText.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStyledText
#define _H_JStyledText

#include "JBroadcaster.h"
#include "JFont.h"
#include "JRunArray.h"
#include "JPtrArray-JString.h"
#include "JStringMatch.h"

class JRegex;
class JInterpolate;
class JUndo;
class JSTUndoBase;
class JSTUndoTextBase;
class JSTUndoTyping;
class JSTUndoStyle;
class JSTUndoPaste;
class JSTUndoTabShift;
class JSTUndoMove;

typedef JBoolean (*JCharacterInWordFn)(const JUtf8Character&);

class JStyledText : virtual public JBroadcaster
{
	friend class JSTUndoTextBase;
	friend class JSTUndoStyle;
	friend class JSTUndoMove;

public:

	enum PlainTextFormat
	{
		kUNIXText,
		kMacintoshText,
		kDOSText
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

		TextCount& operator+=(const TextCount& c);
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

		TextIndex& operator+=(const TextCount& c);
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

		TextRange(const TextIndex& first, const TextIndex& beyondLast)
		{
			charRange.Set(first.charIndex, beyondLast.charIndex-1);
			byteRange.Set(first.byteIndex, beyondLast.byteIndex-1);
		};

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

		TextIndex
		GetFirst() const
		{
			return TextIndex(charRange.first, byteRange.first);
		};

		TextIndex
		GetLast(const JStyledText& text) const
		{
			return text.AdjustTextIndex(GetAfter(), -1);
		}

		TextIndex
		GetAfter() const
		{
			return TextIndex(charRange.last+1, byteRange.last+1);
		}

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
		JRegex*		first;
		JRegex*		rest;
		JString*	replace;

		CRMRule()
			:
			first(nullptr), rest(nullptr), replace(nullptr)
			{ };

		CRMRule(JRegex* f, JRegex* r, JString* repl)
			:
			first(f), rest(r), replace(repl)
			{ };

		CRMRule(const JString& firstPattern, const JString& restPattern,
				const JString& replacePattern);
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

public:

	JStyledText(const JBoolean useMultipleUndo, const JBoolean pasteStyledText);
	JStyledText(const JStyledText& source);

	virtual ~JStyledText();

	JBoolean		IsEmpty() const;
	JBoolean		EndsWithNewline() const;
	const JString&	GetText() const;
	JBoolean		SetText(const JString& text, const JRunArray<JFont>* style = nullptr);

	const JRunArray<JFont>&	GetStyles() const;
	JBoolean				WillPasteStyledText() const;

	JBoolean	IsCharacterInWord(const JUtf8Character& c) const;

	TextIndex	GetWordStart(const TextIndex& index) const;
	TextIndex	GetWordEnd(const TextIndex& index) const;
	TextIndex	GetPartialWordStart(const TextIndex& index) const;
	TextIndex	GetPartialWordEnd(const TextIndex& index) const;
	TextIndex	GetParagraphStart(const TextIndex& index) const;
	TextIndex	GetParagraphEnd(const TextIndex& index) const;
	TextIndex	GetBeyondEnd() const;

	JIndex		GetColumnForChar(const TextIndex& lineStart, const TextIndex& location) const;
	TextIndex	AdjustTextIndex(const TextIndex& index, const JInteger charDelta) const;

	JStringIterator*	GetConstIterator(const JIteratorPosition pos, const TextIndex& index) const;
	void				DisposeConstIterator(JStringIterator* iter) const;

	JBoolean	ReadPlainText(const JString& fileName, PlainTextFormat* format,
							  const JBoolean acceptBinaryFile = kJTrue);
	void		WritePlainText(const JString& fileName, const PlainTextFormat format) const;
	void		WritePlainText(std::ostream& output, const PlainTextFormat format) const;

	JBoolean	ReadPrivateFormat(std::istream& input);
	void		WritePrivateFormat(std::ostream& output) const;

	static JBoolean	ReadPrivateFormat(std::istream& input,
									  JString* text, JRunArray<JFont>* style);
	static void	WritePrivateFormat(std::ostream& output, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange);

	JStringMatch	SearchForward(const TextIndex& startIndex,
								  const JRegex& regex, const JBoolean entireWord,
								  const JBoolean wrapSearch, JBoolean* wrapped);
	JStringMatch	SearchBackward(const TextIndex& startIndex,
								   const JRegex& regex, const JBoolean entireWord,
								   const JBoolean wrapSearch, JBoolean* wrapped);

	TextCount	ReplaceMatch(const JStringMatch& match,
							 const JString& replaceStr, JInterpolate* interpolator,
							 const JBoolean preserveCase,
							 const JBoolean createUndo = kJTrue);
	TextRange	ReplaceAllInRange(const TextRange& range,
								  const JRegex& regex, const JBoolean entireWord,
								  const JString& replaceStr, JInterpolate* interpolator,
								  const JBoolean preserveCase);
	JBoolean	IsEntireWord(const TextRange& range) const;

	JBoolean	SearchForward(std::function<JBoolean(const JFont&)> match,
							  const TextIndex& startIndex,
							  const JBoolean wrapSearch,
							  JBoolean* wrapped, TextRange* range);
	JBoolean	SearchBackward(std::function<JBoolean(const JFont&)> match,
							   const TextIndex& startIndex,
							   const JBoolean wrapSearch,
							   JBoolean* wrapped, TextRange* range);

	JFont		GetFont(const JIndex charIndex) const;

	JBoolean	SetFontName(const TextRange& range,
							const JString& name, const JBoolean clearUndo);
	JBoolean	SetFontSize(const TextRange& range,
							const JSize size, const JBoolean clearUndo);
	JBoolean	SetFontBold(const TextRange& range,
							const JBoolean bold, const JBoolean clearUndo);
	JBoolean	SetFontItalic(const TextRange& range,
							  const JBoolean italic, const JBoolean clearUndo);
	JBoolean	SetFontUnderline(const TextRange& range,
								 const JSize count, const JBoolean clearUndo);
	JBoolean	SetFontStrike(const TextRange& range,
							  const JBoolean strike, const JBoolean clearUndo);
	JBoolean	SetFontColor(const TextRange& range,
							 const JColorID color, const JBoolean clearUndo);
	JBoolean	SetFontStyle(const TextRange& range,
							 const JFontStyle& style, const JBoolean clearUndo);
	void		SetFont(const TextRange& range,
						const JFont& font, const JBoolean clearUndo);

	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const JBoolean clearUndo);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name);
	void	SetDefaultFontSize(const JSize size);
	void	SetDefaultFontStyle(const JFontStyle& style);
	void	SetDefaultFont(const JFont& f);

	JFont	CalcInsertionFont(const TextIndex& index) const;

	TextRange	Outdent(const TextRange& range, const JSize tabCount = 1,
						const JBoolean force = kJFalse);
	TextRange	Indent(const TextRange& range, const JSize tabCount = 1);

	TextRange	CleanWhitespace(const TextRange& range, const JBoolean align);

	JBoolean	TabInsertsSpaces() const;
	void		TabShouldInsertSpaces(const JBoolean spaces);

	JBoolean	WillAutoIndent() const;
	void		ShouldAutoIndent(const JBoolean indent);

	JBoolean	HasSingleUndo() const;
	JBoolean	HasMultipleUndo(JBoolean* canUndo, JBoolean* canRedo) const;
	void		Undo();
	void		Redo();
	void		ClearUndo();
	void		DeactivateCurrentUndo();

	JSize		GetUndoDepth() const;
	void		SetUndoDepth(const JSize maxUndoCount);

	JBoolean	IsAtLastSaveLocation() const;
	void		SetLastSaveLocation();
	void		ClearLastSaveLocation();

	JBoolean	Copy(const TextRange& range,
					 JString* text, JRunArray<JFont>* style = nullptr) const;
	TextRange	Paste(const TextRange& range,
					  const JString& text, const JRunArray<JFont>* style = nullptr);

	JBoolean	MoveText(const TextRange& srcRange, const TextIndex& origDestIndex,
						 const JBoolean copy, TextRange* newRange);

	TextIndex	BackwardDelete(const TextIndex&	lineStart, const TextIndex&	caretIndex,
							   const JBoolean deleteToTabStop,
							   JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr,
							   JUndo** undo = nullptr);
	void		ForwardDelete(const TextIndex& lineStart, const TextIndex& caretIndex,
							  const JBoolean deleteToTabStop,
							  JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr,
							  JUndo** undo = nullptr);
	void		DeleteText(const TextRange& range);

	JUndo*		InsertCharacter(const TextRange& replaceRange,
								const JUtf8Character& key, const JFont& font);

	void		InsertSpacesForTab(const TextIndex& lineStart, const TextIndex& caretIndex);

	static JBoolean	ContainsIllegalChars(const JString& text);
	static JBoolean	RemoveIllegalChars(JString* text, JRunArray<JFont>* style = nullptr);

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

	TextRange	SelectAll() const;
	TextRange	CharToTextRange(const TextIndex* lineStart, const JCharacterRange& charRange) const;

	void		SetBlockSizes(const JSize textBlockSize, const JSize styleBlockSize);

	static JListT::CompareResult
		CompareCharacterIndices(const TextIndex& i, const TextIndex& j);
	static JListT::CompareResult
		CompareByteIndices(const TextIndex& i, const TextIndex& j);

protected:

	JBoolean	IsEntireWord(const JString& text, const TextRange& range) const;

	void	SetFont(const TextRange& range, const JRunArray<JFont>& f);

	void	BroadcastTextChanged(const TextRange& range,
								 const JInteger charDelta, const JInteger byteDelta,
								 const JBoolean deletion,
								 const JBoolean adjustStyles = kJTrue);
	void	BroadcastUndoFinished(const TextRange& range);

	virtual JBoolean	NeedsToFilterText(const JString& text) const;
	virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style);

	virtual JBoolean	NeedsToAdjustFontToDisplayGlyphs(const JString& text, const JRunArray<JFont>& style) const;
	virtual JBoolean	AdjustFontToDisplayGlyphs(const TextRange& range,
												  const JString& text,
												  JRunArray<JFont>* style);

	virtual void	AdjustStylesBeforeBroadcast(const JString& text,
												JRunArray<JFont>* styles,
												TextRange* recalcRange,
												TextRange* redrawRange,
												const JBoolean deletion);

private:

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

private:

	JString				itsText;
	JRunArray<JFont>*	itsStyles;
	const JBoolean		itsPasteStyledTextFlag;		// kJTrue => paste styled text
	JBoolean			itsTabToSpacesFlag;			// kJTrue => 1 tab -> itsCRMTabCharCount spaces
	JBoolean			itsAutoIndentFlag;			// kJTrue => auto-indent after newline

	JFont	itsDefaultFont;

	JSTUndoBase*			itsUndo;				// can be nullptr
	JPtrArray<JSTUndoBase>*	itsUndoList;			// nullptr if not multiple undo
	JIndex					itsFirstRedoIndex;		// range [1:count+1]
	JInteger				itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState				itsUndoState;
	JSize					itsMaxUndoCount;		// maximum length of itsUndoList

	JCharacterInWordFn	itsCharInWordFn;

	JSize	itsCRMLineWidth;
	JSize	itsCRMTabCharCount;

	CRMRuleList*	itsCRMRuleList;		// can be nullptr
	JBoolean		itsOwnsCRMRulesFlag;

private:

	JFont	CalcInsertionFont(JStringIterator& iter,
							  JRunArrayIterator<JFont>& styleIter) const;

	JBoolean			GetCurrentUndo(JSTUndoBase** undo) const;
	JBoolean			GetCurrentRedo(JSTUndoBase** redo) const;
	void				NewUndo(JSTUndoBase* undo, const JBoolean isNew);
	void				ReplaceUndo(JSTUndoBase* oldUndo, JSTUndoBase* newUndo);
	void				ClearOutdatedUndo();
	JSTUndoTyping*		GetTypingUndo(const TextIndex& start, JBoolean* isNew);
	JSTUndoStyle*		GetStyleUndo(const TextRange& range, JBoolean* isNew);
	JSTUndoPaste*		GetPasteUndo(const TextRange& range, JBoolean* isNew);
	JSTUndoTabShift*	GetTabShiftUndo(const TextRange& range, JBoolean* isNew);
	JSTUndoMove*		GetMoveUndo(const TextIndex& srcIndex, const TextIndex& destIndex,
									const TextCount& count, JBoolean* isNew);

	JString	PrepareReplaceMatch(const JStringMatch& match,
								const JString& replaceStr, JInterpolate* interpolator,
								const JBoolean preserveCase);

	TextCount	PrivatePaste(const TextRange& range,
							 const JString& text, const JRunArray<JFont>* style);
	TextCount	InsertText(const TextIndex& index, const JString& text,
						   const JFont& font);
	TextCount	InsertText(const TextIndex& index, const JString& text,
						   const JRunArray<JFont>& style);
	TextCount	InsertText(JStringIterator* targetText, JRunArrayIterator<JFont>* targetStyle,
						   const JString& text, const JRunArray<JFont>& style);
	void		PrivateDeleteText(const TextRange& range);
	void		PrivateDeleteText(JStringIterator* textIter,
								  JRunArrayIterator<JFont>* styleIter,
								  const JSize charCount);
	JBoolean	CleanText(const JString& text, const JRunArray<JFont>& style,
						  JString** cleanText, JRunArray<JFont>** cleanStyle,
						  JBoolean* okToInsert);

	static TextRange	CharToTextRange(const JCharacterRange& charRange,
										JStringIterator* iter);

	void	AutoIndent(JSTUndoTyping* typingUndo);

	static JBoolean	DefaultIsCharacterInWord(const JUtf8Character& c);

	static void	WritePrivateFormat(std::ostream& output, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const TextRange& range);

	static void	ConvertFromMacintoshNewlinetoUNIXNewline(JString* text);

	// not allowed

	const JStyledText& operator=(const JStyledText& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kTextSet;
	static const JUtf8Byte* kTextChanged;
	static const JUtf8Byte* kDefaultFontChanged;
	static const JUtf8Byte* kUndoFinished;
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

		TextChanged(const TextRange& r, const TextRange& rr,
					const JInteger cd, const JInteger bd)
			:
			JBroadcaster::Message(kTextChanged),
			itsRange(r),
			itsRedrawRange(rr),
			itsCharDelta(cd),
			itsByteDelta(bd)
			{ };

		const TextRange&
		GetRange() const
		{
			return itsRange;
		}

		const TextRange&
		GetRedrawRange() const
		{
			return itsRedrawRange;
		}

		const JInteger
		GetCharDelta() const
		{
			return itsCharDelta;
		}

		const JInteger
		GetByteDelta() const
		{
			return itsByteDelta;
		}

	private:

		const TextRange	itsRange;
		const TextRange	itsRedrawRange;		// may be larger than itsRange
		const JInteger	itsCharDelta;
		const JInteger	itsByteDelta;
	};

	class DefaultFontChanged : public JBroadcaster::Message
	{
	public:

		DefaultFontChanged()
			:
			JBroadcaster::Message(kDefaultFontChanged)
			{ };
	};

	class UndoFinished : public JBroadcaster::Message
	{
	public:

		UndoFinished(const TextRange& r)
			:
			JBroadcaster::Message(kUndoFinished),
			itsRange(r)
			{ };

		const TextRange&
		GetRange() const
		{
			return itsRange;
		}

	private:

		const TextRange	itsRange;
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
JStyledText::IsEmpty()
	const
{
	return itsText.IsEmpty();
}

/******************************************************************************
 GetText

 ******************************************************************************/

inline const JString&
JStyledText::GetText()
	const
{
	return itsText;
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the given character range is a single, complete word.

 ******************************************************************************/

inline JBoolean
JStyledText::IsEntireWord
	(
	const TextRange& range
	)
	const
{
	return IsEntireWord(itsText, range);
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

inline JStyledText::TextRange
JStyledText::SelectAll()
	const
{
	return TextRange(JCharacterRange(1, itsText.GetCharacterCount()),
					 JUtf8ByteRange(1, itsText.GetByteCount()));
}

/******************************************************************************
 GetStyles

 ******************************************************************************/

inline const JRunArray<JFont>&
JStyledText::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 Has undo

 ******************************************************************************/

inline JBoolean
JStyledText::HasSingleUndo()
	const
{
	return JI2B( itsUndo != nullptr );
}

inline JBoolean
JStyledText::HasMultipleUndo
	(
	JBoolean* canUndo,
	JBoolean* canRedo
	)
	const
{
	*canUndo = JI2B( itsFirstRedoIndex > 1 );
	*canRedo = JI2B( itsUndoList != nullptr && itsFirstRedoIndex <= itsUndoList->GetElementCount() );
	return JI2B( itsUndoList != nullptr );
}

/******************************************************************************
 GetUndoDepth

 ******************************************************************************/

inline JSize
JStyledText::GetUndoDepth()
	const
{
	return itsMaxUndoCount;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline JBoolean
JStyledText::IsAtLastSaveLocation()
	const
{
	return JI2B( itsUndoList != nullptr &&
				 itsLastSaveRedoIndex > 0 &&
				 JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex );
}

inline void
JStyledText::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
JStyledText::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

/******************************************************************************
 WillPasteStyledText

 ******************************************************************************/

inline JBoolean
JStyledText::WillPasteStyledText()
	const
{
	return itsPasteStyledTextFlag;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline const JFont&
JStyledText::GetDefaultFont()
	const
{
	return itsDefaultFont;
}

/******************************************************************************
 Tab inserts spaces

 ******************************************************************************/

inline JBoolean
JStyledText::TabInsertsSpaces()
	const
{
	return itsTabToSpacesFlag;
}

inline void
JStyledText::TabShouldInsertSpaces
	(
	const JBoolean spaces
	)
{
	itsTabToSpacesFlag = spaces;
}

/******************************************************************************
 Auto indenting

 ******************************************************************************/

inline JBoolean
JStyledText::WillAutoIndent()
	const
{
	return itsAutoIndentFlag;
}

inline void
JStyledText::ShouldAutoIndent
	(
	const JBoolean indent
	)
{
	itsAutoIndentFlag = indent;
}

/******************************************************************************
 CleanRightMargin() parameters

 ******************************************************************************/

inline JSize
JStyledText::GetCRMLineWidth()
	const
{
	return itsCRMLineWidth;
}

inline JSize
JStyledText::GetCRMTabCharCount()
	const
{
	return itsCRMTabCharCount;
}

inline JBoolean
JStyledText::GetCRMRuleList
	(
	const CRMRuleList** ruleList
	)
	const
{
	*ruleList = itsCRMRuleList;
	return JI2B( itsCRMRuleList != nullptr );
}

/******************************************************************************
 Font access

 ******************************************************************************/

inline JFont
JStyledText::GetFont
	(
	const JIndex charIndex
	)
	const
{
	JRunArrayIterator<JFont> iter(*itsStyles, kJIteratorStartBefore, charIndex);
	JFont f;
	iter.Next(&f);
	return f;
}

inline void
JStyledText::SetDefaultFontName
	(
	const JString& name
	)
{
	itsDefaultFont.SetName(name);
	Broadcast(DefaultFontChanged());
}

inline void
JStyledText::SetDefaultFontSize
	(
	const JSize size
	)
{
	itsDefaultFont.SetSize(size);
	Broadcast(DefaultFontChanged());
}

inline void
JStyledText::SetDefaultFontStyle
	(
	const JFontStyle& style
	)
{
	itsDefaultFont.SetStyle(style);
	Broadcast(DefaultFontChanged());
}

inline void
JStyledText::SetDefaultFont
	(
	const JFont& font
	)
{
	itsDefaultFont = font;
	Broadcast(DefaultFontChanged());
}

/******************************************************************************
 EndsWithNewline

 ******************************************************************************/

inline JBoolean
JStyledText::EndsWithNewline()
	const
{
	return JI2B(!itsText.IsEmpty() &&
				itsText.GetLastCharacter() == '\n' );
}

/******************************************************************************
 GetBeyondEnd

 ******************************************************************************/

inline JStyledText::TextIndex
JStyledText::GetBeyondEnd()
	const
{
	return TextIndex(itsText.GetCharacterCount()+1, itsText.GetByteCount()+1);
}

/******************************************************************************
 GetCharacterInWordFunction

 ******************************************************************************/

inline JCharacterInWordFn
JStyledText::GetCharacterInWordFunction()
	const
{
	return itsCharInWordFn;
}

/******************************************************************************
 GetCharacterInWordFunction

 ******************************************************************************/

inline void
JStyledText::SetBlockSizes
	(
	const JSize textBlockSize,
	const JSize styleBlockSize
	)
{
	itsText.SetBlockSize(textBlockSize);
	itsStyles->SetBlockSize(styleBlockSize);
}

/******************************************************************************
 TextIndex operators

 ******************************************************************************/

inline JStyledText::TextIndex
operator+
	(
	const JStyledText::TextIndex& i,
	const JStyledText::TextCount& c
	)
{
	return JStyledText::TextIndex(i.charIndex + c.charCount, i.byteIndex + c.byteCount);
}

inline JStyledText::TextIndex&
JStyledText::TextIndex::operator+=
	(
	const JStyledText::TextCount& c
	)
{
	charIndex += c.charCount;
	byteIndex += c.byteCount;
	return *this;
}

/******************************************************************************
 TextCount operators

 ******************************************************************************/

inline JStyledText::TextCount
operator+
	(
	const JStyledText::TextCount& c1,
	const JStyledText::TextCount& c2
	)
{
	return JStyledText::TextCount(c1.charCount + c2.charCount, c1.byteCount + c2.byteCount);
}

inline JStyledText::TextCount&
JStyledText::TextCount::operator+=
	(
	const JStyledText::TextCount& c
	)
{
	charCount += c.charCount;
	byteCount += c.byteCount;
	return *this;
}

/******************************************************************************
 Catch attempts to stream JStyledText instead of JString (link error)

 ******************************************************************************/

std::istream& operator>>(std::istream& input, JStyledText* text);
std::ostream& operator<<(std::ostream& input, const JStyledText* text);

#endif
