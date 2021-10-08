/******************************************************************************
 JXCreatePG.h

	Interface for JXCreatePG class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCreatePG
#define _H_JXCreatePG

#include <jx-af/jcore/JCreateProgressDisplay.h>

class JXCreatePG : public JCreateProgressDisplay
{
public:

	JXCreatePG();

	~JXCreatePG() override;

	JProgressDisplay* New() override;
};

#endif
