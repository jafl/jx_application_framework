/******************************************************************************
 RecordDataField.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_RecordDataField
#define _H_RecordDataField

#include <jx-af/jx/JXInputField.h>

class RecordDataField : public JXInputField
{
public:

	RecordDataField(const JString& text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~RecordDataField();
};

#endif
