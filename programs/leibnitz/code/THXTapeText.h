/******************************************************************************
 THXTapeText.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXTapeText
#define _H_THXTapeText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>

class THXTapeText : public JXTEBase
{
public:

	THXTapeText(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~THXTapeText();

private:

	// not allowed

	THXTapeText(const THXTapeText& source);
	const THXTapeText& operator=(const THXTapeText& source);
};

#endif
