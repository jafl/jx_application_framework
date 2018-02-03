/******************************************************************************
 GNBTreeWidgetDragData.h

	Interface for the GNBTreeWidgetDragData class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GNBTreeWidgetDragData
#define _H_GNBTreeWidgetDragData


#include <JXSelectionData.h>



class GNBTreeWidgetDragData : public JXSelectionData
{
public:

	GNBTreeWidgetDragData(JXWidget* widget, const JCharacter* id);
	virtual ~GNBTreeWidgetDragData();

protected:

			virtual void		AddTypes(const Atom selectionName);
			virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
											unsigned char** data, JSize* dataLength,
											JSize* bitsPerBlock) const;
private:

	// not allowed

	GNBTreeWidgetDragData(const GNBTreeWidgetDragData& source);
	const GNBTreeWidgetDragData& operator=(const GNBTreeWidgetDragData& source);

public:
};

#endif
