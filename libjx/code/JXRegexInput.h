/******************************************************************************
 JXRegexInput.h

	Interface for the JXRegexInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRegexInput
#define _H_JXRegexInput

#include "jx-af/jx/JXInputField.h"

class JRegex;

class JXRegexInput : public JXInputField
{
public:

	JXRegexInput(JRegex* testRegex, const bool widgetOwnsRegex,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXRegexInput() override;

	bool	InputValid() override;

private:

	JRegex*	itsTestRegex;
	bool	itsOwnsRegexFlag;
};

#endif
