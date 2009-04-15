/******************************************************************************
 JXStyleMenuTable.h

	Interface for the JXStyleMenuTable class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStyleMenuTable
#define _H_JXStyleMenuTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenuTable.h>

class JXStyleMenu;

class JXStyleMenuTable : public JXTextMenuTable
{
public:

	JXStyleMenuTable(JXStyleMenu* menu, JXTextMenuData* data, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXStyleMenuTable();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

private:

	JXStyleMenu*	itsStyleMenu;	// it owns us

private:

	// not allowed

	JXStyleMenuTable(const JXStyleMenuTable& source);
	const JXStyleMenuTable& operator=(const JXStyleMenuTable& source);
};

#endif
