/******************************************************************************
 CMDebuggerProgramInput.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDebuggerProgramInput
#define _H_CMDebuggerProgramInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFileInput.h>

class CMDebuggerProgramInput : public JXFileInput
{
public:

	CMDebuggerProgramInput(JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h);

	virtual ~CMDebuggerProgramInput();

	virtual JBoolean	InputValid();
	virtual JBoolean	GetFile(JString* fullName) const;

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<Font>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);
private:

	// not allowed

	CMDebuggerProgramInput(const CMDebuggerProgramInput& source);
	const CMDebuggerProgramInput& operator=(const CMDebuggerProgramInput& source);
};

#endif
