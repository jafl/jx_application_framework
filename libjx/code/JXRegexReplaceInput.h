/******************************************************************************
 JXRegexReplaceInput.h

	Interface for the JXRegexReplaceInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRegexReplaceInput
#define _H_JXRegexReplaceInput

#include "JXInputField.h"

class JInterpolate;

class JXRegexReplaceInput : public JXInputField
{
public:

	JXRegexReplaceInput(JInterpolate* testInterpolator, const JBoolean widgetOwnsInterpolator,
						JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXRegexReplaceInput();

	virtual JBoolean	InputValid();

private:

	JInterpolate*	itsTestInterpolator;
	JBoolean		itsOwnsInterpolatorFlag;

private:

	// not allowed

	JXRegexReplaceInput(const JXRegexReplaceInput& source);
	const JXRegexReplaceInput& operator=(const JXRegexReplaceInput& source);
};

#endif
