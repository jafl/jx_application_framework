/******************************************************************************
 GFilterDragData.h

	Interface for the GFilterDragData class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GFilterDragData
#define _H_GFilterDragData

#include <jx-af/jx/JXSelectionData.h>

class GFilterDragData : public JXSelectionData
{
public:

	GFilterDragData(JXWidget* widget, const JUtf8Byte* id);

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;
};

#endif
