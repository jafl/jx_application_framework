/******************************************************************************
 JXDockDragData.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockDragData
#define _H_JXDockDragData

#include "JXSelectionManager.h"

class JXWindow;

class JXDockDragData : public JXSelectionData
{
public:

	JXDockDragData(JXWindow* window);

	~JXDockDragData() override;

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	JXWindow*	itsWindow;
};

#endif
