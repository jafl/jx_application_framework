/******************************************************************************
 MDRecordDataField.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDRecordDataField
#define _H_MDRecordDataField

#include <jx-af/jx/JXInputField.h>

class MDRecordDataField : public JXInputField
{
public:

	MDRecordDataField(const JString& text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~MDRecordDataField();
};

#endif
