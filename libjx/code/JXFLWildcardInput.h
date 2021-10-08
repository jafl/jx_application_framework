/******************************************************************************
 JXFLWildcardInput.h

	Interface for the JXFLWildcardInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFLWildcardInput
#define _H_JXFLWildcardInput

#include "jx-af/jx/JXFLInputBase.h"

class JXFLWildcardInput : public JXFLInputBase
{
public:

	JXFLWildcardInput(JXFileListSet* flSet, JXStringHistoryMenu* historyMenu,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~JXFLWildcardInput();

	JString	GetRegexString() const override;
};

#endif
