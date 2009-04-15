/******************************************************************************
 JTEStyler.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEStyler
#define _H_JTEStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTextEditor.h>	// need def of Font

class JTEStyler
{
public:

	// Feel free to request other items in this union.  It is only here
	// because certain exceptional languages like TCL and PHP require
	// additional state.  We don't provide a void* because then we would
	// have to keep track of whether or not it needs to be deleted.

	union TokenExtra
	{
		JIndex			indexValue;
		JSize			sizeValue;
		yy_state_type	lexerState;
	};

	struct TokenData
	{
		JIndex		startIndex;
		TokenExtra	data;

		TokenData()
			:
			startIndex(0), data()
		{ };

		TokenData(const JIndex i, const TokenExtra& d)
			:
			startIndex(i), data(d)
		{ };
	};

public:

	JTEStyler();

	virtual ~JTEStyler();

	static JArray<TokenData>*	NewTokenStartList();

	void	UpdateStyles(const JTextEditor* te,
						 const JString& text, JRunArray<JTextEditor::Font>* styles,
						 JIndexRange* recalcRange, JIndexRange* redrawRange,
						 const JBoolean deletion, JArray<TokenData>* tokenStartList);

	JBoolean	IsActive() const;
	void		SetActive(const JBoolean active);

protected:

	virtual void		Scan(istream& input, const TokenExtra& initData) = 0;
	virtual TokenExtra	GetFirstTokenExtraData() const;
	virtual void		PreexpandCheckRange(const JString& text,
											const JRunArray<JTextEditor::Font>& styles,
											const JIndexRange& modifiedRange,
											const JBoolean deletion,
											JIndexRange* checkRange);
	void				ExtendCheckRange(const JIndex newEndIndex);

	const JTextEditor*					GetTextEditor() const;
	const JFontManager*					GetFontManager() const;
	const JFontStyle&					GetDefaultFontStyle() const;
	const JString&						GetText() const;
	const JRunArray<JTextEditor::Font>&	GetStyles() const;

	JBoolean	SetStyle(const JIndexRange& range, const JFontStyle& style);
	void		SaveTokenStart(const TokenExtra& data);

	void	AdjustStyle(const JIndexRange& range, const JFontStyle& style);

private:

	JBoolean	itsActiveFlag;

	const JTextEditor*				itsTE;			// not owned; NULL unless lexing
	const JFontManager*				itsFontMgr;		// not owned; NULL unless lexing
	const JString*					itsText;		// not owned; NULL unless lexing
	JRunArray<JTextEditor::Font>*	itsStyles;		// not owned; NULL unless lexing

	JBoolean	itsRedoAllFlag;						// kJTrue => itsStyles is *not* full
	JString		itsFontName;
	JSize		itsFontSize;
	JFontStyle	itsDefFontStyle;
	JFontID		itsDefFontID;

	JIndexRange*	itsRecalcRange;					// not owned; NULL unless lexing
	JIndexRange*	itsRedrawRange;					// not owned; NULL unless lexing
	JIndexRange		itsCheckRange;

	JArray<TokenData>*	itsTokenStartList;			// not owned; NULL unless lexing
	JSize				itsTokenStartFactor;
	JSize				itsTokenStartCount;

	// information about next token that will be received

	JIndex	itsTokenStart;
	JIndex	itsTokenRunIndex;						// invalid when itsRedoAllFlag
	JIndex	itsTokenFirstInRun;						// invalid when itsRedoAllFlag

private:

	JBoolean	OnlyColorChanged(JFontStyle s1, JFontStyle s2) const;

	static JOrderedSetT::CompareResult
		CompareTokenStarts(const TokenData& t1, const TokenData& t2);

	// not allowed

	JTEStyler(const JTEStyler& source);
	const JTEStyler& operator=(const JTEStyler& source);
};


/******************************************************************************
 Active

 ******************************************************************************/

inline JBoolean
JTEStyler::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JTEStyler::SetActive
	(
	const JBoolean active
	)
{
	itsActiveFlag = active;
}

/******************************************************************************
 GetTextEditor (protected)

 ******************************************************************************/

inline const JTextEditor*
JTEStyler::GetTextEditor()
	const
{
	return itsTE;
}

/******************************************************************************
 GetFontManager (protected)

 ******************************************************************************/

inline const JFontManager*
JTEStyler::GetFontManager()
	const
{
	return itsFontMgr;
}

/******************************************************************************
 GetDefaultFontStyle (protected)

 ******************************************************************************/

inline const JFontStyle&
JTEStyler::GetDefaultFontStyle()
	const
{
	return itsDefFontStyle;
}

/******************************************************************************
 GetText (protected)

 ******************************************************************************/

inline const JString&
JTEStyler::GetText()
	const
{
	return *itsText;
}

/******************************************************************************
 GetStyles (protected)

 ******************************************************************************/

inline const JRunArray<JTextEditor::Font>&
JTEStyler::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 OnlyColorChanged (private)

	We don't use JFontStyle& because we modify the values.

 ******************************************************************************/

inline JBoolean
JTEStyler::OnlyColorChanged
	(
	JFontStyle s1,
	JFontStyle s2
	)
	const
{
	s1.color = s2.color = 0;	// make sure the color is the same
	return JI2B(s1 == s2);		// avoids maintenance when fields are added
}

#endif
