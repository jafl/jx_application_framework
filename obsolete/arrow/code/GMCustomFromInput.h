/******************************************************************************
 GMCustomFromInput.h

	Interface for the GMCustomFromInput class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMCustomFromInput
#define _H_GMCustomFromInput

#include <JXInputField.h>

class JXTextCheckbox;

class GMCustomFromInput : public JXInputField
{
public:

	GMCustomFromInput(JXTextCheckbox* cb, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);
	virtual ~GMCustomFromInput();

protected:

	virtual JBoolean	InputValid();

private:

	JXTextCheckbox*		itsControlCB;

private:

	// not allowed

	GMCustomFromInput(const GMCustomFromInput& source);
	const GMCustomFromInput& operator=(const GMCustomFromInput& source);
};

#endif
