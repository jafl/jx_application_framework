/******************************************************************************
 CBStylerBase.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBStylerBase
#define _H_CBStylerBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEStyler.h>
#include <JPrefObject.h>
#include <JStringMap.h>
#include "CBTextFileType.h"

class CBEditStylerDialog;

class CBStylerBase : public JTEStyler, public JPrefObject, virtual public JBroadcaster
{
public:

	typedef JStrValue<JFontStyle>	WordStyle;

public:

	CBStylerBase(const JSize typeVersion,
				 const JSize typeCount, const JCharacter** typeNames,
				 const JCharacter* editDialogTitle, const JPrefID& prefID,
				 const CBTextFileType fileType);

	virtual ~CBStylerBase();

	void	EditStyles();

	JColormap*						GetColormap() const;
	const JStringMap<JFontStyle>&	GetWordList() const;

	void	ReadFromSharedPrefs(istream& input);
	void	WriteForSharedPrefs(ostream& output) const;

protected:

	JFontStyle	GetStyle(const JIndex typeIndex, const JString& word);

	JFontStyle	GetTypeStyle(const JIndex index) const;
	void		SetTypeStyle(const JIndex index, const JFontStyle& style);

	JBoolean	GetWordStyle(const JString& word, JFontStyle* style) const;
	void		SetWordStyle(const JCharacter* word, const JFontStyle& style);
	void		RemoveWordStyle(const JCharacter* word);

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) = 0;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JColormap*				itsColormap;			// not owned
	JColorIndex				itsDefColor;

	const JFileVersion		itsTypeNameVersion;
	const JSize				itsTypeNameCount;
	const JCharacter**		itsTypeNames;			// not owned
	JArray<JFontStyle>*		itsTypeStyles;
	const CBTextFileType	itsFileType;			// only use for IsCharInWord() !!

	JStringMap<JFontStyle>*	itsWordStyles;

	const JCharacter*		itsDialogTitle;			// not owned
	CBEditStylerDialog*		itsEditDialog;			// can be NULL
	JString					itsDialogGeom;

private:

	void	GetWordList(const JStringMap<JFontStyle>& wordStyles,
						JArray<WordStyle>* wordList,
						const JBoolean sort) const;

	JFontStyle	ReadStyle(istream& input);
	void		WriteStyle(ostream& output, const JFontStyle& style) const;

	void		ExtractStyles();
	JBoolean	TypeStylesChanged(const JArray<JFontStyle>& newTypeStyles) const;
	JBoolean	WordStylesChanged(const JStringMap<JFontStyle>& newWordStyles) const;

	void	SetDefaultFontColor(const JColorIndex color);

	static JOrderedSetT::CompareResult
		CompareWords(const WordStyle& w1, const WordStyle& w2);

	// not allowed

	CBStylerBase(const CBStylerBase& source);
	const CBStylerBase& operator=(const CBStylerBase& source);

public:

	// JBroadcaster messages

	static const JCharacter* kWordListChanged;

	class WordListChanged : public JBroadcaster::Message
		{
		public:

			WordListChanged(const JStringMap<JFontStyle>& wordList)
				:
				JBroadcaster::Message(kWordListChanged),
				itsWordList(wordList)
				{ };

			const JStringMap<JFontStyle>&
			GetWordList() const
			{
				return itsWordList;
			};

		private:

			const JStringMap<JFontStyle>&	itsWordList;
		};
};


/******************************************************************************
 GetWordList

 ******************************************************************************/

inline const JStringMap<JFontStyle>&
CBStylerBase::GetWordList()
	const
{
	return *itsWordStyles;
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JColormap*
CBStylerBase::GetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 Type style (protected)

 ******************************************************************************/

inline JFontStyle
CBStylerBase::GetTypeStyle
	(
	const JIndex index
	)
	const
{
	return itsTypeStyles->GetElement(index);
}

inline void
CBStylerBase::SetTypeStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	itsTypeStyles->SetElement(index, style);
}

/******************************************************************************
 Word style (protected)

 ******************************************************************************/

inline JBoolean
CBStylerBase::GetWordStyle
	(
	const JString&	word,
	JFontStyle*		style
	)
	const
{
	return itsWordStyles->GetElement(word, style);
}

inline void
CBStylerBase::SetWordStyle
	(
	const JCharacter*	word,
	const JFontStyle&	style
	)
{
	itsWordStyles->SetElement(word, style);
}

inline void
CBStylerBase::RemoveWordStyle
	(
	const JCharacter* word
	)
{
	itsWordStyles->RemoveElement(word);
}

/******************************************************************************
 Shared prefs

 ******************************************************************************/

inline void
CBStylerBase::ReadFromSharedPrefs
	(
	istream& input
	)
{
	ReadPrefs(input);
}

inline void
CBStylerBase::WriteForSharedPrefs
	(
	ostream& output
	)
	const
{
	WritePrefs(output);
}

#endif
