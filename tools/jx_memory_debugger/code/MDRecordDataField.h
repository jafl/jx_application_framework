/******************************************************************************
 MDRecordDataField.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDRecordDataField
#define _H_MDRecordDataField

#include <JXInputField.h>

class MDRecordDataField : public JXInputField
{
public:

	MDRecordDataField(const JString& text, JXContainer* enclosure,
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
