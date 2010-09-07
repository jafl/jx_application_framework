/******************************************************************************
 MDRecordDataField.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_MDRecordDataField
#define _H_MDRecordDataField

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class MDRecordDataField : public JXInputField
{
public:

	MDRecordDataField(const JCharacter* text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~MDRecordDataField();

private:

	// not allowed

	MDRecordDataField(const MDRecordDataField& source);
	const MDRecordDataField& operator=(const MDRecordDataField& source);
};

#endif
