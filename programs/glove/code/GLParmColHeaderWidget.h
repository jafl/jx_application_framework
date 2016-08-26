/******************************************************************************
 GLParmColHeaderWidget.h

	Interface for the GLParmColHeaderWidget class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GLParmColHeaderWidget
#define _H_GLParmColHeaderWidget


#include <JXColHeaderWidget.h>

class JPagePrinter;

class GLParmColHeaderWidget : public JXColHeaderWidget
{
public:

public:

	GLParmColHeaderWidget(JXTable* table, JXScrollbarSet* scrollbarSet,
						  JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	virtual ~GLParmColHeaderWidget();

	void	PrintOnPage(JPagePrinter& p);

private:

	// not allowed

	GLParmColHeaderWidget(const GLParmColHeaderWidget& source);
	const GLParmColHeaderWidget& operator=(const GLParmColHeaderWidget& source);

public:
};

#endif
