/******************************************************************************
 JXRegexReplaceInput.h

	Interface for the JXRegexReplaceInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXRegexReplaceInput
#define _H_JXRegexReplaceInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JRegex;

class JXRegexReplaceInput : public JXInputField
{
public:

	JXRegexReplaceInput(JRegex* testRegex, const JBoolean widgetOwnsRegex,
						JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXRegexReplaceInput();

	virtual JBoolean	InputValid();

private:

	JRegex*		itsTestRegex;
	JBoolean	itsOwnsRegexFlag;

private:

	// not allowed

	JXRegexReplaceInput(const JXRegexReplaceInput& source);
	const JXRegexReplaceInput& operator=(const JXRegexReplaceInput& source);
};

#endif
