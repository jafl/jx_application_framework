/******************************************************************************
 SyGFilterInput.h

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFilterInput
#define _H_SyGFilterInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>
#include <JString.h>

class SyGFilterInput : public JXInputField
{
public:

	SyGFilterInput(JXWidget* widget, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~SyGFilterInput();

	virtual void HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleFocusEvent();			// must call inherited

private:

	JXWidget*	itsDirList;
	JString		itsInitialText;

	// not allowed

	SyGFilterInput(const SyGFilterInput& source);
	const SyGFilterInput& operator=(const SyGFilterInput& source);
};

#endif
