/******************************************************************************
 JStyleTableData.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStyleTableData
#define _H_JStyleTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JAuxTableData.h>
#include <JFontStyle.h>
#include <JString.h>

class JFontManager;
class JColormap;

class JStyleTableData : public JAuxTableData<JFontStyle>
{
public:

	JStyleTableData(JTable* table, const JFontManager* fontManager, JColormap* colormap);

	virtual ~JStyleTableData();

	const JString&	GetFont(JSize* size) const;
	void			SetFont(const JCharacter* name, const JSize size);

	JFontStyle	GetCellStyle(const JPoint& cell) const;
	void		SetCellStyle(const JPoint& cell, const JFontStyle& style);
	void		SetAllCellStyles(const JFontStyle& style);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JString		itsFontName;
	JSize		itsFontSize;
	JBoolean	itsProcessMessagesFlag;

	const JFontManager*	itsFontManager;			// not owned
	JColormap*			itsColormap;			// not owned

private:

	void	UpdateAllColors(const JBoolean allocate);
	void	UpdateRectColors(const JRect& rect, const JBoolean allocate);

	// not allowed

	JStyleTableData(const JStyleTableData& source);
	const JStyleTableData& operator=(const JStyleTableData& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontChanged;

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };
		};
};


/******************************************************************************
 GetFont

 ******************************************************************************/

inline const JString&
JStyleTableData::GetFont
	(
	JSize* size
	)
	const
{
	*size = itsFontSize;
	return itsFontName;
}

/******************************************************************************
 GetCellStyle

 ******************************************************************************/

inline JFontStyle
JStyleTableData::GetCellStyle
	(
	const JPoint& cell
	)
	const
{
	return GetElement(cell);
}

#endif
