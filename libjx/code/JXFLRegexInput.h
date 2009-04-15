/******************************************************************************
 JXFLRegexInput.h

	Interface for the JXFLRegexInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFLRegexInput
#define _H_JXFLRegexInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFLInputBase.h>

class JXFLRegexInput : public JXFLInputBase
{
public:

	JXFLRegexInput(JXFileListSet* flSet, JXStringHistoryMenu* historyMenu,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXFLRegexInput();

	virtual JString	GetRegexString() const;

private:

	// not allowed

	JXFLRegexInput(const JXFLRegexInput& source);
	const JXFLRegexInput& operator=(const JXFLRegexInput& source);
};

#endif
