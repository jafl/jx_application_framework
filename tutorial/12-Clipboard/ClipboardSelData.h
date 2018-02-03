/******************************************************************************
 GFilterDragData.h

	Interface for the GFilterDragData class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GFilterDragData
#define _H_GFilterDragData


#include <JXSelectionManager.h>

class GFilterDragData : public JXSelectionData
{
public:

	GFilterDragData(JXWidget* widget, const JCharacter* id);
	virtual ~GFilterDragData();

protected:

			virtual void		AddTypes(const Atom selectionName);
			virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
											unsigned char** data, JSize* dataLength,
											JSize* bitsPerBlock) const;
private:

	// not allowed

	GFilterDragData(const GFilterDragData& source);
	const GFilterDragData& operator=(const GFilterDragData& source);

public:
};

#endif
