/******************************************************************************
 JXFLWildcardInput.h

	Interface for the JXFLWildcardInput class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFLWildcardInput
#define _H_JXFLWildcardInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFLInputBase.h>

class JXFLWildcardInput : public JXFLInputBase
{
public:

	JXFLWildcardInput(JXFileListSet* flSet, JXStringHistoryMenu* historyMenu,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXFLWildcardInput();

	virtual JString	GetRegexString() const;

private:

	// not allowed

	JXFLWildcardInput(const JXFLWildcardInput& source);
	const JXFLWildcardInput& operator=(const JXFLWildcardInput& source);
};

#endif
