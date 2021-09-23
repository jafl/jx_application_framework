/******************************************************************************
 JXCreatePG.h

	Interface for JXCreatePG class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCreatePG
#define _H_JXCreatePG

#include <JCreateProgressDisplay.h>

class JXCreatePG : public JCreateProgressDisplay
{
public:

	JXCreatePG();

	virtual ~JXCreatePG();

	virtual JProgressDisplay* New();
};

#endif
