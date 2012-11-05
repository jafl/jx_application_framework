/******************************************************************************
 CBKeyScriptInput.h

	Interface for the CBKeyScriptInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBKeyScriptInput
#define _H_CBKeyScriptInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class CBKeyScriptInput : public JXInputField
{
public:

	CBKeyScriptInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CBKeyScriptInput();

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<Font>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	// not allowed

	CBKeyScriptInput(const CBKeyScriptInput& source);
	const CBKeyScriptInput& operator=(const CBKeyScriptInput& source);
};

#endif
