/******************************************************************************
 JXStringList.h

	Interface for the JXStringList class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStringList
#define _H_JXStringList

#include <JXTable.h>
#include <JRunArray.h>
#include <JAliasArray.h>
#include <JFont.h>

class JString;
class JStyleTableData;

class JXStringList : public JXTable
{
public:

	JXStringList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXStringList();

	const JPtrArray<JString>&	GetStringList() const;
	void						SetStringList(const JPtrArray<JString>* list);

	const JFont&	GetFont() const;
	void			SetFont(const JString& name, const JSize size);

	JFontStyle	GetStyle(const JIndex index) const;
	void		SetStyle(const JIndex index, const JFontStyle& style);
	void		SetStyles(const JRunArray<JFontStyle>& styleList);
	void		SetAllStyles(const JFontStyle& style);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;
	void			ClearIncrementalSearchBuffer();

	// called by urgent task

	void	AdjustColWidth();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	HandleFocusEvent() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	const JPtrArray<JString>*	itsList;	// not owned
	JStyleTableData*			itsStyles;
	JCoordinate					itsMinColWidth;

	JAliasArray<JString*>*	itsSortedList;	// NULL if itsList == NULL
	JString					itsKeyBuffer;

private:

	JBoolean	ClosestMatch(const JString& prefixStr, JIndex* index) const;

	// not allowed

	JXStringList(const JXStringList& source);
	const JXStringList& operator=(const JXStringList& source);
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
