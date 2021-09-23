/******************************************************************************
 JXDSSSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDSSSelection
#define _H_JXDSSSelection

#include "JXSelectionManager.h"

class JXDirectSaveAction;

class JXDSSSelection : public JXSelectionData
{
public:

	JXDSSSelection(JXWindow* window, JXDirectSaveAction* action);

	virtual	~JXDSSSelection();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JXWindow*			itsWindow;		// not owned
	JXDirectSaveAction*	itsAction;		// nullptr after first transaction
};

#endif
