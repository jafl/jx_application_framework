/******************************************************************************
 JXImageMenuTable.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMenuTable
#define _H_JXImageMenuTable

#include "jx-af/jx/JXMenuTable.h"
#include "jx-af/jx/jXConstants.h"	// need definition of kJXDefaultBorderWidth

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

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual bool	CellToItemIndex(const JPoint& pt, const JPoint& cell,
										JIndex* itemIndex) const override;
	virtual void		MenuHilightItem(const JIndex itemIndex) override;
	virtual void		MenuUnhilightItem(const JIndex itemIndex) override;
	virtual void		GetSubmenuPoints(const JIndex itemIndex,
										 JPoint* leftPt, JPoint* rightPt) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JXImageMenuData*	itsIconMenuData;
	bool				itsHasCheckboxesFlag;
	bool				itsHasSubmenusFlag;
	JPoint					itsHilightCell;

private:

	JPoint	ItemIndexToCell(const JIndex itemIndex) const;
};

#endif
