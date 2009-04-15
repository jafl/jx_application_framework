/******************************************************************************
 JXDSSSelection.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDSSSelection
#define _H_JXDSSSelection

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionManager.h>

class JXDirectSaveAction;

class JXDSSSelection : public JXSelectionData
{
public:

	JXDSSSelection(JXWindow* window, JXDirectSaveAction* action);

	virtual	~JXDSSSelection();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JXWindow*			itsWindow;		// not owned
	JXDirectSaveAction*	itsAction;		// NULL after first transaction

private:

	// not allowed

	JXDSSSelection(const JXDSSSelection& source);
	const JXDSSSelection& operator=(const JXDSSSelection& source);
};

#endif
