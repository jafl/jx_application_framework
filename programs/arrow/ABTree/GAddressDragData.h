/******************************************************************************
 GAddressDragData.h

	Interface for the GAddressDragData class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GAddressDragData
#define _H_GAddressDragData


#include <JXSelectionManager.h>

class GAddressDragData : public JXSelectionData
{
public:

	GAddressDragData(JXWidget* widget, const JCharacter* id);
	virtual ~GAddressDragData();

protected:

			virtual void		AddTypes(const Atom selectionName);
			virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
											unsigned char** data, JSize* dataLength,
											JSize* bitsPerBlock) const;
private:

	// not allowed

	GAddressDragData(const GAddressDragData& source);
	const GAddressDragData& operator=(const GAddressDragData& source);

public:
};

#endif
