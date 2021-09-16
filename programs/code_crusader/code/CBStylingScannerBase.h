/******************************************************************************
 CBStylingScannerBase.h

	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBStylingScannerBase
#define _H_CBStylingScannerBase

#include <reflex/abslexer.h>
#include <reflex/matcher.h>
#include <JStyledText.h>

class CBStylingScannerBase : public reflex::AbstractLexer<reflex::Matcher>
{
public:

	enum BaseTokenType
	{
		kEOF = 258
	};

	struct Token
	{
		int						type;
		JStyledText::TextRange	range;
		JStyledText::TextRange	docCommentRange;	// preceding comment range for DOC comment tags
		const JString*			scriptLanguage;

		Token()
			:
			type(kEOF), scriptLanguage(nullptr)
			{ };

		Token(const int t, const JStyledText::TextRange& r)
			:
			type(t), range(r), scriptLanguage(nullptr)
			{ };

		Token(const int t, const JStyledText::TextRange& r, const JString* l)
			:
			type(t), range(r), scriptLanguage(l)
			{ };
	};

public:

	CBStylingScannerBase(const reflex::Input& input, std::ostream& os);

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

	const JStyledText::TextRange&	GetCurrentRange() const;
	const JStyledText::TextRange&	GetPPNameRange() const;

protected:

	void	InitToken();
	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const int type);
	Token	DocToken(const int type);

	void	Undo(const JStyledText::TextRange& range,
				 const JSize prevCharByteCount, const JString& text);

	void	SetCurrentRange(const JStyledText::TextRange& r);

	void	SavePPNameRange();
	JString	GetPPCommand(const JString& text) const;

	bool	SlurpQuoted(const JSize count, const JUtf8Byte* suffixList);

	bool	IsQuote(const JUtf8Character& c);
private:

	const JStyledText*		itsCurrentText;
	JStyledText::TextRange	itsCurrentRange;
	JStyledText::TextRange	itsPPNameRange;

private:

	bool	ReadCharacter(JStyledText::TextIndex* index, JUtf8Character* ch);

	// not allowed

	CBStylingScannerBase(const CBStylingScannerBase& source);
	const CBStylingScannerBase& operator=(const CBStylingScannerBase& source);
};


/******************************************************************************
 InitToken (protected)

 *****************************************************************************/

inline void
CBStylingScannerBase::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (protected)

 *****************************************************************************/

inline void
CBStylingScannerBase::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (protected)

 *****************************************************************************/

inline void
CBStylingScannerBase::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(text(), size());
	itsCurrentRange.byteRange.last += size();
}

/******************************************************************************
 ThisToken (protected)

 *****************************************************************************/

inline CBStylingScannerBase::Token
CBStylingScannerBase::ThisToken
	(
	const int type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 GetCurrentRange

 *****************************************************************************/

inline const JStyledText::TextRange&
CBStylingScannerBase::GetCurrentRange()
	const
{
	return itsCurrentRange;
}

/******************************************************************************
 SetCurrentRange (protected)

 *****************************************************************************/

inline void
CBStylingScannerBase::SetCurrentRange
	(
	const JStyledText::TextRange& r
	)
{
	itsCurrentRange = r;
}

/******************************************************************************
 GetPPNameRange

 *****************************************************************************/

inline const JStyledText::TextRange&
CBStylingScannerBase::GetPPNameRange()
	const
{
	return itsPPNameRange;
}

/******************************************************************************
 GetPPNameRange (protected)

 *****************************************************************************/

inline bool
CBStylingScannerBase::IsQuote
	(
	const JUtf8Character& c
	)
{
	return c == '\'' || c == '"' || c == '`';
}

/******************************************************************************
 Comparisons

	These compare the type and *range*, not the contents, since ranges do not
	know where they came from.  Still useful when the tokens come from the same
	string.

 *****************************************************************************/

inline int
operator==
	(
	const CBStylingScannerBase::Token& t1,
	const CBStylingScannerBase::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange || t1.type == CBStylingScannerBase::kEOF));
}

inline int
operator!=
	(
	const CBStylingScannerBase::Token& t1,
	const CBStylingScannerBase::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
