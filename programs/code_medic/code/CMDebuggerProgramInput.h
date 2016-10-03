/******************************************************************************
 CMDebuggerProgramInput.h

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDebuggerProgramInput
#define _H_CMDebuggerProgramInput

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

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);
private:

	// not allowed

	CMDebuggerProgramInput(const CMDebuggerProgramInput& source);
	const CMDebuggerProgramInput& operator=(const CMDebuggerProgramInput& source);
};

#endif
