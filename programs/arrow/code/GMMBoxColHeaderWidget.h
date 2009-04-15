/******************************************************************************
 GMMBoxColHeaderWidget.h

	Interface for the GMMBoxColHeaderWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMMBoxColHeaderWidget
#define _H_GMMBoxColHeaderWidget

#include <JXTable.h>

class JString;
class JXScrollbar;

class GMMBoxColHeaderWidget : public JXTable
{
public:

	GMMBoxColHeaderWidget(JXTable* table,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GMMBoxColHeaderWidget();

	JBoolean	GetColTitle(const JIndex index, JString* title) const;
	void		SetColTitle(const JIndex index, const JCharacter* title);
	void		ClearColTitle(const JIndex index);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTable*			itsTable;		// we don't own this
	JPtrArray<JString>*	itsTitles;		// can be NULL; elements can be NULL

private:

	void	AdjustToTable();

	// not allowed

	GMMBoxColHeaderWidget(const GMMBoxColHeaderWidget& source);
	const GMMBoxColHeaderWidget& operator=(const GMMBoxColHeaderWidget& source);
};

#endif
