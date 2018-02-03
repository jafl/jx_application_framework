/******************************************************************************
 GLParmColHeaderWidget.cpp

	BASE CLASS = public JXColHeaderWidget

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLParmColHeaderWidget.h>
#include <JPagePrinter.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLParmColHeaderWidget::GLParmColHeaderWidget
	(
	JXTable*			table,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXColHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLParmColHeaderWidget::~GLParmColHeaderWidget()
{
}

/******************************************************************************
 PrintOnPage (public)

 ******************************************************************************/

void
GLParmColHeaderWidget::PrintOnPage
	(
	JPagePrinter& p
	)
{
	TableDraw(p, GetBounds());
}
