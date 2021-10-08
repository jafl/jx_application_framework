/******************************************************************************
 JCreateTextPG.h

	Interface for JCreateTextPG class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JCreateTextPG
#define _H_JCreateTextPG

#include "jx-af/jcore/JCreateProgressDisplay.h"

class JCreateTextPG : public JCreateProgressDisplay
{
public:

	JCreateTextPG();

	~JCreateTextPG();

	JProgressDisplay* New() override;
};

#endif
