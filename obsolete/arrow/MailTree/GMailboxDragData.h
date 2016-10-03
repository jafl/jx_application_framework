/******************************************************************************
 GMailboxDragData.h

	Interface for the GMailboxDragData class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMailboxDragData
#define _H_GMailboxDragData


#include <JXSelectionManager.h>

class GMailboxDragData : public JXSelectionData
{
public:

	GMailboxDragData(JXWidget* widget, const JCharacter* id);
	virtual ~GMailboxDragData();

protected:

			virtual void		AddTypes(const Atom selectionName);
			virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
											unsigned char** data, JSize* dataLength,
											JSize* bitsPerBlock) const;
private:

	// not allowed

	GMailboxDragData(const GMailboxDragData& source);
	const GMailboxDragData& operator=(const GMailboxDragData& source);

public:
};

#endif
