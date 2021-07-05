/******************************************************************************
 CBCmdLineInput.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCmdLineInput
#define _H_CBCmdLineInput

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

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	CBExecOutputDocument*	itsDoc;

private:

	// not allowed

	CBCmdLineInput(const CBCmdLineInput& source);
	const CBCmdLineInput& operator=(const CBCmdLineInput& source);
};

#endif
