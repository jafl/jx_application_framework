/******************************************************************************
 JXImageMenuTable.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageMenuTable
#define _H_JXImageMenuTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMenuTable.h>
#include <jXConstants.h>	// need definition of kJXDefaultBorderWidth

class JXImageMenuData;

class JXImageMenuTable : public JXMenuTable
{
public:

	enum
	{
		kHilightBorderWidth = kJXDefaultBorderWidth
	};

public:

	JXImageMenuTable(JXMenu* menu, JXImageMenuData* data, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXImageMenuTable();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual JBoolean	CellToItemIndex(const JPoint& pt, const JPoint& cell,
										JIndex* itemIndex) const;
	virtual void		MenuHilightItem(const JIndex itemIndex);
	virtual void		MenuUnhilightItem(const JIndex itemIndex);
	virtual void		GetSubmenuPoints(const JIndex itemIndex,
										 JPoint* leftPt, JPoint* rightPt);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const JXImageMenuData*	itsIconMenuData;
	JBoolean				itsHasCheckboxesFlag;
	JBoolean				itsHasSubmenusFlag;
	JPoint					itsHilightCell;

private:

	JPoint	ItemIndexToCell(const JIndex itemIndex) const;

	// not allowed

	JXImageMenuTable(const JXImageMenuTable& source);
	const JXImageMenuTable& operator=(const JXImageMenuTable& source);
};

#endif
