/******************************************************************************
 SCEditVarValueDialog.h

	Interface for the SCEditVarValueDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCEditVarValueDialog
#define _H_SCEditVarValueDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class SCCircuitVarList;
class JXFloatInput;

class SCEditVarValueDialog : public JXDialogDirector
{
public:

	SCEditVarValueDialog(JXWindowDirector* supervisor, SCCircuitVarList* varList,
						 const JIndex varIndex);

	virtual ~SCEditVarValueDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	SCCircuitVarList*	itsVarList;
	const JIndex		itsVarIndex;

// begin JXLayout

	JXFloatInput* itsValue;

// end JXLayout

private:

	void	BuildWindow(SCCircuitVarList* varList, const JIndex varIndex);

	// not allowed

	SCEditVarValueDialog(const SCEditVarValueDialog& source);
	const SCEditVarValueDialog& operator=(const SCEditVarValueDialog& source);
};

#endif
