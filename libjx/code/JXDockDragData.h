/******************************************************************************
 JXDockDragData.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockDragData
#define _H_JXDockDragData

#include "jx-af/jx/JXSelectionManager.h"

class JXWindow;

class JXDockDragData : public JXSelectionData
{
public:

	JXDockDragData(JXWindow* window);

	virtual	~JXDockDragData();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JXWindow*	itsWindow;
};

#endif
