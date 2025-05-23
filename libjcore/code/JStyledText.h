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
#include "JStringIterator.h"
#include "JStringMatch.h"

class JRegex;
class JInterpolate;
class JUndoRedoChain;
class JUndo;
class JSTUndoBase;
class JSTUndoTextBase;
class JSTUndoTyping;
class JSTUndoStyle;
class JSTUndoPaste;
class JSTUndoTabShift;
class JSTUndoMove;

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
		JSize charCount;
		JSize byteCount;

		TextCount()
			:
			charCount(0),
			byteCount(0)
			{ };

		TextCount(const JSize ch, const JSize byte)
			:
			charCount(ch),
			byteCount(byte)
			{ };

		void
		Set(const JSize ch, const JSize byte)
		{
			charCount = ch;
			byteCount = byte;
		}

		TextCount& operator+=(const TextCount& c);
		TextCount& operator-=(const TextCount& c);
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

		void
		Set(const JIndex ch, const JIndex byte)
		{
			charIndex = ch;
			byteIndex = byte;
		}

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
			Set(first, beyondLast);
		};

		TextRange(const TextIndex& i, const TextCount& c)
		{
			Set(i, c);
		};

		bool
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

		void
		Set(const JCharacterRange& cr, const JUtf8ByteRange& br)
		{
			charRange = cr;
			byteRange = br;
		}

		void
		Set(const TextIndex& first, const TextIndex& beyondLast)
		{
			charRange.Set(first.charIndex, beyondLast.charIndex-1);
			byteRange.Set(first.byteIndex, beyondLast.byteIndex-1);
		}

		void
		Set(const TextIndex& i, const TextCount& c)
		{
			charRange.SetFirstAndCount(i.charIndex, c.charCount);
			byteRange.SetFirstAndCount(i.byteIndex, c.byteCount);
		}

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

		TextRange& operator+=(const TextRange& r);
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

		void	CleanOut();

		static JRegex*			CreateRegex(const JString& pattern);
		static JInterpolate*	CreateInterpolator();
	};

	class CRMRuleList : public JArray<CRMRule>
	{
	public:

		CRMRuleList();
		CRMRuleList(const CRMRuleList& source);

		~CRMRuleList() override;

		JInterpolate*
		GetInterpolator()
		{
			return itsInterpolator;
		};

		void	DeleteAll();

	private:

		JInterpolate*	itsInterpolator;
	};

public:

	JStyledText(const bool useMultipleUndo, const bool pasteStyledText);
	JStyledText(const JStyledText& source);

	~JStyledText() override;

	bool			IsEmpty() const;
	bool			EndsWithNewline() const;
	const JString&	GetText() const;
	bool			SetText(const JString& text, const JRunArray<JFont>* style = nullptr);

	const JRunArray<JFont>&	GetStyles() const;
	bool					WillPasteStyledText() const;
	void					RestyleAll();

	bool	IsCharacterInWord(const JUtf8Character& c) const;

	TextIndex	GetWordStart(const TextIndex& index) const;
	TextIndex	GetWordEnd(const TextIndex& index) const;
	TextIndex	GetPartialWordStart(const TextIndex& index) const;
	TextIndex	GetPartialWordEnd(const TextIndex& index) const;
	TextIndex	GetParagraphStart(const TextIndex& index) const;
	TextIndex	GetParagraphEnd(const TextIndex& index) const;
	TextIndex	GetBeyondEnd() const;

	JIndex		GetColumnForChar(const TextIndex& lineStart, const TextIndex& location) const;
	TextIndex	AdjustTextIndex(const TextIndex& index, const JInteger charDelta) const;

	JStringIterator*	GetConstIterator(const JStringIterator::Position pos, const TextIndex& index) const;
	void				DisposeConstIterator(JStringIterator* iter) const;

	bool	ReadPlainText(const JString& fileName, PlainTextFormat* format,
						  const bool acceptBinaryFile = true);
	void	WritePlainText(const JString& fileName, const PlainTextFormat format) const;
	void	WritePlainText(std::ostream& output, const PlainTextFormat format) const;

	bool	ReadPrivateFormat(std::istream& input);
	void	WritePrivateFormat(std::ostream& output) const;

	static bool	ReadPrivateFormat(std::istream& input,
								  JString* text, JRunArray<JFont>* style);
	static void	WritePrivateFormat(std::ostream& output, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const JCharacterRange& charRange);

	JStringMatch	SearchForward(const TextIndex& startIndex,
								  const JRegex& regex, const bool entireWord,
								  const bool wrapSearch, bool* wrapped) const;
	JStringMatch	SearchBackward(const TextIndex& startIndex,
								   const JRegex& regex, const bool entireWord,
								   const bool wrapSearch, bool* wrapped) const;

	TextCount	ReplaceMatch(const JStringMatch& match,
							 const JString& replaceStr, JInterpolate* interpolator,
							 const bool preserveCase,
							 const bool createUndo = true);
	TextRange	ReplaceAllInRange(const TextRange& range,
								  const JRegex& regex, const bool entireWord,
								  const JString& replaceStr, JInterpolate* interpolator,
								  const bool preserveCase);
	bool	IsEntireWord(const TextRange& range) const;

	bool	SearchForward(std::function<bool(const JFont&)> match,
							  const TextIndex& startIndex,
							  const bool wrapSearch,
							  bool* wrapped, TextRange* range) const;
	bool	SearchBackward(std::function<bool(const JFont&)> match,
							   const TextIndex& startIndex,
							   const bool wrapSearch,
							   bool* wrapped, TextRange* range) const;

	JFont	GetFont(const JIndex charIndex) const;

	bool	SetFontName(const TextRange& range,
						const JString& name, const bool clearUndo);
	bool	SetFontSize(const TextRange& range,
						const JSize size, const bool clearUndo);
	bool	SetFontBold(const TextRange& range,
						const bool bold, const bool clearUndo);
	bool	SetFontItalic(const TextRange& range,
						  const bool italic, const bool clearUndo);
	bool	SetFontUnderline(const TextRange& range,
							 const JSize count, const bool clearUndo);
	bool	SetFontStrike(const TextRange& range,
						  const bool strike, const bool clearUndo);
	bool	SetFontColor(const TextRange& range,
						 const JColorID color, const bool clearUndo);
	bool	SetFontStyle(const TextRange& range,
						 const JFontStyle& style, const bool clearUndo);
	void	SetFont(const TextRange& range,
					const JFont& font, const bool clearUndo);

	void	SetAllFontNameAndSize(const JString& name, const JSize size,
								  const bool clearUndo);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name);
	void	SetDefaultFontSize(const JSize size);
	void	SetDefaultFontStyle(const JFontStyle& style);
	void	SetDefaultFont(const JFont& f);

	JFont	CalcInsertionFont(const TextIndex& index) const;

	TextRange	Outdent(const TextRange& range, const JSize tabCount = 1,
						const bool force = false);
	TextRange	Indent(const TextRange& range, const JSize tabCount = 1);

	TextRange	CleanWhitespace(const TextRange& range, const bool align);

	bool	TabInsertsSpaces() const;
	void	TabShouldInsertSpaces(const bool spaces = true);

	bool	WillAutoIndent() const;
	void	ShouldAutoIndent(const bool indent = true);

	void	Undo();
	void	Redo();
	void	ClearUndo();
	void	DeactivateCurrentUndo();

	JUndoRedoChain*			GetUndoRedoChain();
	const JUndoRedoChain*	GetUndoRedoChain() const;

	bool		Copy(const TextRange& range,
					 JString* text, JRunArray<JFont>* style = nullptr) const;
	TextRange	Paste(const TextRange& range,
					  const JString& text, const JRunArray<JFont>* style = nullptr);

	bool		MoveText(const TextRange& srcRange, const TextIndex& origDestIndex,
						 const bool copy, TextRange* newRange);

	TextIndex	BackwardDelete(const TextIndex&	lineStart, const TextIndex&	caretIndex,
							   const bool deleteToTabStop,
							   JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr,
							   JUndo** undo = nullptr);
	void		ForwardDelete(const TextIndex& lineStart, const TextIndex& caretIndex,
							  const bool deleteToTabStop,
							  JString* returnText = nullptr, JRunArray<JFont>* returnStyle = nullptr,
							  JUndo** undo = nullptr);
	void		DeleteText(const TextRange& range);

	JUndo*	InsertCharacter(const TextRange& replaceRange,
							const JUtf8Character& key, const JFont& font,
							TextCount* count);

	JStyledText::TextRange	InsertSpacesForTab(const TextIndex& lineStart, const TextRange& replaceRange);

	static bool	ContainsIllegalChars(const JString& text);
	static bool	RemoveIllegalChars(JString* text, JRunArray<JFont>* style = nullptr);

	bool	CleanRightMargin(TextIndex* caretIndex,
							 const TextRange& selectionRange,
							 const bool coerce);

	JSize	GetCRMLineWidth() const;
	void	SetCRMLineWidth(const JSize charCount);

	JSize	GetCRMTabCharCount() const;
	void	SetCRMTabCharCount(const JSize charCount);
	JSize	CRMGetTabWidth(const JIndex textColumn) const;

	bool	GetCRMRuleList(const CRMRuleList** ruleList) const;
	void	SetCRMRuleList(CRMRuleList* ruleList,
						   const bool teOwnsRuleList);
	void	ClearCRMRuleList();

	void	SetCharacterInWordFunction(const std::function<bool(const JUtf8Character&)> f);

	TextRange	SelectAll() const;
	TextRange	CharToTextRange(const TextIndex* lineStart, const JCharacterRange& charRange) const;

	void		SetLgMinSizes(const JSize textLgSize, const JSize styleLgSize);

	static std::weak_ordering
		CompareCharacterIndices(const TextIndex& i, const TextIndex& j);
	static std::weak_ordering
		CompareByteIndices(const TextIndex& i, const TextIndex& j);

	// JTextEditor

	bool	CRMGetPrefix(TextIndex* startIndex, const TextIndex& beyondEndIndex,
						 JString* linePrefix, JSize* columnCount,
						 JIndex* ruleIndex) const;
	bool	CRMLineMatchesRest(const TextRange& range) const;

protected:

	bool	IsEntireWord(const JString& text, const TextRange& range) const;

	void	SetFont(const TextRange& range, const JRunArray<JFont>& f);

	void	BroadcastTextChanged(const TextRange& range,
								 const JInteger charDelta, const JInteger byteDelta,
								 const bool deletion,
								 const bool adjustStyles = true);
	void	BroadcastUndoFinished(const TextRange& range);

	virtual bool	NeedsToFilterText(const JString& text) const;
	virtual bool	FilterText(JString* text, JRunArray<JFont>* style);

	virtual bool	NeedsToAdjustFontToDisplayGlyphs(const JString& text, const JRunArray<JFont>& style) const;
	virtual bool	AdjustFontToDisplayGlyphs(const TextRange& range,
											  const JString& text,
											  JRunArray<JFont>* style);

	virtual void	AdjustStylesBeforeBroadcast(const JString& text,
												JRunArray<JFont>* styles,
												TextRange* recalcRange,
												TextRange* redrawRange,
												const bool deletion);

private:

	enum CRMStatus
	{
		kFoundWord,
		kFoundNewLine,
		kFinished
	};

private:

	JString				itsText;
	JRunArray<JFont>*	itsStyles;
	JUndoRedoChain*		itsUndoChain;
	const bool			itsPasteStyledTextFlag;		// true => paste styled text
	bool				itsTabToSpacesFlag;			// true => 1 tab -> itsCRMTabCharCount spaces
	bool				itsAutoIndentFlag;			// true => auto-indent after newline

	JFont	itsDefaultFont;

	std::function<bool(const JUtf8Character&)>*	itsCharInWordFn;	// null => default

	JSize	itsCRMLineWidth;
	JSize	itsCRMTabCharCount;

	CRMRuleList*	itsCRMRuleList;		// can be nullptr
	bool			itsOwnsCRMRulesFlag;

private:

	JFont	CalcInsertionFont(JStringIterator& iter,
							  JRunArrayIterator<JFont>& styleIter) const;

	JSTUndoTyping*		GetTypingUndo(const TextIndex& start, bool* isNew);
	JSTUndoStyle*		GetStyleUndo(const TextRange& range, bool* isNew);
	JSTUndoPaste*		GetPasteUndo(const TextRange& range, bool* isNew);
	JSTUndoTabShift*	GetTabShiftUndo(const TextRange& range, bool* isNew);
	JSTUndoMove*		GetMoveUndo(const TextIndex& srcIndex, const TextIndex& destIndex,
									const TextCount& count, bool* isNew);

	JString	PrepareReplaceMatch(const JStringMatch& match,
								const JString& replaceStr, JInterpolate* interpolator,
								const bool preserveCase) const;

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
	bool		CleanText(const JString& text, const JRunArray<JFont>& style,
						  JString** cleanText, JRunArray<JFont>** cleanStyle,
						  bool* okToInsert);

	static TextRange	CharToTextRange(const JCharacterRange& charRange,
										JStringIterator* iter);

	void	AutoIndent(JSTUndoTyping* typingUndo, const TextIndex& insertIndex,
					   TextCount* count);

	bool		PrivateCleanRightMargin(const TextIndex& start, const bool coerce,
										TextRange* textRange,
										JString* newText, JRunArray<JFont>* newStyles,
										TextIndex* newCaretIndex) const;
	bool		CRMGetRange(const TextIndex& caretIndex, const bool coerce,
							TextRange* range, TextIndex* textStartIndex,
							JString* firstLinePrefix, JSize* firstPrefixLength,
							JString* restLinePrefix, JSize* restPrefixLength,
							JIndex* returnRuleIndex) const;
	TextRange	CRMMatchPrefix(const TextRange& textRange, JIndex* ruleIndex) const;
	JSize		CRMCalcColumnCount(const JString& linePrefix) const;
	JString		CRMBuildRestPrefix(const JString& firstLinePrefix,
								   const JIndex ruleIndex, JSize* columnCount) const;
	void		CRMTossLinePrefix(TextIndex* charIndex, const TextIndex& beyondEndIndex,
								  const JIndex ruleIndex) const;
	CRMStatus	CRMReadNextWord(TextIndex* charIndex, const JIndex endCharIndex,
								JString* spaceBuffer, JSize* spaceCount,
								JString* wordBuffer, JRunArray<JFont>* wordStyles,
								const JSize currentCharCount, const TextIndex& origCaretIndex,
								bool* hasTotalCount, TextCount* totalCount,
								const JString& newText, const bool requireSpace) const;
	void		CRMAppendWord(JString* newText, JRunArray<JFont>* newStyles,
							  JSize* currentCharCount,
							  bool* hasTotalCount, TextCount* totalCount,
							  const JString& spaceBuffer, const JSize spaceCount,
							  const JString& wordBuffer, const JRunArray<JFont>& wordStyles,
							  const JString& linePrefix, const JSize prefixLength) const;

	static bool	DefaultIsCharacterInWord(const JUtf8Character& c);

	static void	WritePrivateFormat(std::ostream& output, const JFileVersion vers,
								   const JString& text, const JRunArray<JFont>& style,
								   const TextRange& range);

	static void	ConvertFromMacintoshNewlinetoUNIXNewline(JString* text);

	// not allowed

	JStyledText& operator=(const JStyledText&) = delete;

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

		TextChanged(const TextRange& r, const TextRange& rcr, const TextRange& rdr,
					const JInteger cd, const JInteger bd)
			:
			JBroadcaster::Message(kTextChanged),
			itsRange(r),
			itsRecalcRange(rcr),
			itsRedrawRange(rdr),
			itsCharDelta(cd),
			itsByteDelta(bd)
			{ };

		const TextRange&
		GetRange() const
		{
			return itsRange;
		}

		const TextRange&
		GetRecalcRange() const
		{
			return itsRecalcRange;
		}

		const TextRange&
		GetRedrawRange() const
		{
			return itsRedrawRange;
		}

		JInteger
		GetCharDelta() const
		{
			return itsCharDelta;
		}

		JInteger
		GetByteDelta() const
		{
			return itsByteDelta;
		}

	private:

		const TextRange	itsRange;
		const TextRange	itsRecalcRange;		// with auto-styling, typically larger than itsRange
		const TextRange	itsRedrawRange;		// may be larger than itsRecalcRange
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

inline bool
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

	Return true if the given character range is a single, complete word.

 ******************************************************************************/

inline bool
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
 Undo chain

 ******************************************************************************/

inline JUndoRedoChain*
JStyledText::GetUndoRedoChain()
{
	return itsUndoChain;
}

inline const JUndoRedoChain*
JStyledText::GetUndoRedoChain()
	const
{
	return itsUndoChain;
}

/******************************************************************************
 WillPasteStyledText

 ******************************************************************************/

inline bool
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

inline bool
JStyledText::TabInsertsSpaces()
	const
{
	return itsTabToSpacesFlag;
}

inline void
JStyledText::TabShouldInsertSpaces
	(
	const bool spaces
	)
{
	itsTabToSpacesFlag = spaces;
}

/******************************************************************************
 Auto indenting

 ******************************************************************************/

inline bool
JStyledText::WillAutoIndent()
	const
{
	return itsAutoIndentFlag;
}

inline void
JStyledText::ShouldAutoIndent
	(
	const bool indent
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

inline bool
JStyledText::GetCRMRuleList
	(
	const CRMRuleList** ruleList
	)
	const
{
	*ruleList = itsCRMRuleList;
	return itsCRMRuleList != nullptr;
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
	JRunArrayIterator iter(*itsStyles, JListT::kStartBefore, charIndex);
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

inline bool
JStyledText::EndsWithNewline()
	const
{
	return !itsText.IsEmpty() && itsText.GetLastCharacter() == '\n';
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
 SetLgMinSizes

 ******************************************************************************/

inline void
JStyledText::SetLgMinSizes
	(
	const JSize textLgSize,
	const JSize styleLgSize
	)
{
	itsText.SetMinLgSize(textLgSize);
	itsStyles->SetMinLgSize(styleLgSize);
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
 TextRange operators

 ******************************************************************************/

inline JStyledText::TextRange&
JStyledText::TextRange::operator+=
	(
	const JStyledText::TextRange& r
	)
{
	charRange += r.charRange;
	byteRange += r.byteRange;
	return *this;
}

/******************************************************************************
 Catch attempts to stream JStyledText instead of JString (link error)

 ******************************************************************************/

std::istream& operator>>(std::istream& input, JStyledText* text);
std::ostream& operator<<(std::ostream& input, const JStyledText* text);

#endif
