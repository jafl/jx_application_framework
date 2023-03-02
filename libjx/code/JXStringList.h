/******************************************************************************
 JXStringList.h

	Interface for the JXStringList class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStringList
#define _H_JXStringList

#include "JXTable.h"
#include <jx-af/jcore/JRunArray.h>
#include <jx-af/jcore/JAliasArray.h>
#include <jx-af/jcore/JFont.h>

class JString;
class JStyleTableData;

class JXStringList : public JXTable
{
public:

	JXStringList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXStringList() override;

	const JPtrArray<JString>&	GetStringList() const;
	void						SetStringList(const JPtrArray<JString>* list);

	const JFont&	GetFont() const;
	void			SetFont(const JString& name, const JSize size);

	JFontStyle	GetStyle(const JIndex index) const;
	void		SetStyle(const JIndex index, const JFontStyle& style);
	void		SetStyles(const JRunArray<JFontStyle>& styleList);
	void		SetAllStyles(const JFontStyle& style);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	void			ClearIncrementalSearchBuffer();

	// called by urgent task

	void	AdjustColWidth();

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	HandleFocusEvent() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	const JPtrArray<JString>*	itsList;	// not owned
	JStyleTableData*			itsStyles;
	JCoordinate					itsMinColWidth;

	JAliasArray<JString*>*	itsSortedList;	// nullptr if itsList == nullptr
	JString					itsKeyBuffer;

private:

	bool	ClosestMatch(const JString& prefixStr, JIndex* index) const;
};


/******************************************************************************
 GetStringList

 ******************************************************************************/

inline const JPtrArray<JString>&
JXStringList::GetStringList()
	const
{
	return *itsList;
}

#endif
