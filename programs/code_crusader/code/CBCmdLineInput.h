/******************************************************************************
 CBCmdLineInput.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCmdLineInput
#define _H_CBCmdLineInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class CBExecOutputDocument;

class CBCmdLineInput : public JXInputField
{
public:

	CBCmdLineInput(CBExecOutputDocument* doc, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CBCmdLineInput();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	CBExecOutputDocument*	itsDoc;

private:

	// not allowed

	CBCmdLineInput(const CBCmdLineInput& source);
	const CBCmdLineInput& operator=(const CBCmdLineInput& source);
};

#endif
