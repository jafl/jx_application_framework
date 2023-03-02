/******************************************************************************
 JXFLRegexInput.h

	Interface for the JXFLRegexInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFLRegexInput
#define _H_JXFLRegexInput

#include "JXFLInputBase.h"

class JXFLRegexInput : public JXFLInputBase
{
public:

	JXFLRegexInput(JXFileListSet* flSet, JXStringHistoryMenu* historyMenu,
				   JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~JXFLRegexInput() override;

	JString	GetRegexString() const override;
};

#endif
