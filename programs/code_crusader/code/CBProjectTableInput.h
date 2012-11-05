/******************************************************************************
 CBProjectTableInput.h

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBProjectTableInput
#define _H_CBProjectTableInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFileInput.h>

class CBProjectTable;

class CBProjectTableInput : public JXFileInput
{
public:

	CBProjectTableInput(CBProjectTable* table, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~CBProjectTableInput();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	CBProjectTable*	itsProjectTable;

private:

	// not allowed

	CBProjectTableInput(const CBProjectTableInput& source);
	const CBProjectTableInput& operator=(const CBProjectTableInput& source);
};

#endif
