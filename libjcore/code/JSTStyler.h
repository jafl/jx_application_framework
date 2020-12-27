/******************************************************************************
 JSTStyler.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSTStyler
#define _H_JSTStyler

#include "JStyledText.h"
#include <FlexLexer.h>

class JSTStyler
{
public:

	// Feel free to request other items in this union.  It is only here
	// because certain exceptional languages like TCL and PHP require
	// additional state.  We don't provide a void* because then we would
	// have to keep track of whether or not it needs to be deleted.

	union TokenExtra
	{
//		JIndex			indexValue;
//		JSize			sizeValue;
		yy_state_type	lexerState;
	};

	struct TokenData
	{
		JStyledText::TextIndex	startIndex;
		TokenExtra				data;

		TokenData()
			:
			data()
		{ };

		TokenData(const JStyledText::TextIndex& i, const TokenExtra& d)
			:
			startIndex(i), data(d)
		{ };
	};

public:

	JSTStyler();

	virtual ~JSTStyler();

	static JArray<TokenData>*	NewTokenStartList();

	void	UpdateStyles(const JStyledText* te,
						 const JString& text, JRunArray<JFont>* styles,
						 JStyledText::TextRange* recalcRange,
						 JStyledText::TextRange* redrawRange,
						 const JBoolean deletion, JArray<TokenData>* tokenStartList);

	JBoolean	IsActive() const;
	void		SetActive(const JBoolean active);

protected:

	virtual void		Scan(const JStyledText::TextIndex& startIndex,
							 std::istream& input, const TokenExtra& initData) = 0;
	virtual TokenExtra	GetFirstTokenExtraData() const;
	virtual void		PreexpandCheckRange(const JString& text,
											const JRunArray<JFont>& styles,
											const JCharacterRange& modifiedRange,
											const JBoolean deletion,
											JStyledText::TextRange* checkRange);
	void				ExtendCheckRange(const JIndex newEndCharIndex);

	const JStyledText*		GetStyledText() const;
	const JFontManager*		GetFontManager() const;
	const JFont&			GetDefaultFont() const;
	const JString&			GetText() const;
	const JRunArray<JFont>&	GetStyles() const;

	JUtf8Character	GetCharacter(const JStyledText::TextIndex& index) const;

	JBoolean	SetStyle(const JCharacterRange& range, const JFontStyle& style);
	void		SaveTokenStart(const JStyledText::TextIndex& index, const TokenExtra data = TokenExtra());

	void	AdjustStyle(const JCharacterRange& range, const JFontStyle& style);

	// testing

	void	SetDecimationFactor(const JSize factor);

private:

	JBoolean	itsActiveFlag;

	const JStyledText*	itsST;			// not owned; nullptr unless lexing
	const JFontManager*	itsFontMgr;		// not owned; nullptr unless lexing
	const JString*		itsText;		// not owned; nullptr unless lexing
	JRunArray<JFont>*	itsStyles;		// not owned; nullptr unless lexing

	JBoolean	itsRedoAllFlag;						// kJTrue => itsStyles is *not* full
	JFont*		itsDefFont;							// nullptr unless processing

	JStyledText::TextRange*	itsRecalcRange;		// not owned; nullptr unless lexing
	JStyledText::TextRange*	itsRedrawRange;		// not owned; nullptr unless lexing
	JStyledText::TextRange	itsCheckRange;

	JArray<TokenData>*	itsTokenStartList;			// not owned; nullptr unless lexing
	JSize				itsTokenStartCounter;

	JRunArrayIterator<JFont>*	itsIterator;		// nullptr unless lexing

	JSize	itsDecimationFactor;

private:

	JBoolean	OnlyColorChanged(JFontStyle s1, JFontStyle s2) const;

	void	ExpandTextRange(JStyledText::TextRange* r1, const JCharacterRange& r2) const;

	static JListT::CompareResult
		CompareTokenStarts(const TokenData& t1, const TokenData& t2);

	// not allowed

	JSTStyler(const JSTStyler& source);
	const JSTStyler& operator=(const JSTStyler& source);
};


/******************************************************************************
 Active

 ******************************************************************************/

inline JBoolean
JSTStyler::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JSTStyler::SetActive
	(
	const JBoolean active
	)
{
	itsActiveFlag = active;
}

/******************************************************************************
 GetStyledText (protected)

 ******************************************************************************/

inline const JStyledText*
JSTStyler::GetStyledText()
	const
{
	return itsST;
}

/******************************************************************************
 GetDefaultFont (protected)

 ******************************************************************************/

inline const JFont&
JSTStyler::GetDefaultFont()
	const
{
	return *itsDefFont;
}

/******************************************************************************
 GetText (protected)

 ******************************************************************************/

inline const JString&
JSTStyler::GetText()
	const
{
	return *itsText;
}

/******************************************************************************
 GetStyles (protected)

 ******************************************************************************/

inline const JRunArray<JFont>&
JSTStyler::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 SetDecimationFactor (protected)

 ******************************************************************************/

inline void
JSTStyler::SetDecimationFactor
	(
	const JSize factor
	)
{
	itsDecimationFactor = factor;
}

#endif
