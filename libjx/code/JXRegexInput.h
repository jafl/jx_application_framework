/******************************************************************************
 JXRegexInput.h

	Interface for the JXRegexInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRegexInput
#define _H_JXRegexInput

#include "JXInputField.h"

class JRegex;

class JXRegexInput : public JXInputField
{
public:

	JXRegexInput(JRegex* testRegex, const JBoolean widgetOwnsRegex,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXRegexInput();

	virtual JBoolean	InputValid();

private:

	JRegex*		itsTestRegex;
	JBoolean	itsOwnsRegexFlag;

private:

	// not allowed

	JXRegexInput(const JXRegexInput& source);
	const JXRegexInput& operator=(const JXRegexInput& source);
};

#endif
