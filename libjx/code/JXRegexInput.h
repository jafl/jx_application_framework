/******************************************************************************
 JXRegexInput.h

	Interface for the JXRegexInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXRegexInput
#define _H_JXRegexInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

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
