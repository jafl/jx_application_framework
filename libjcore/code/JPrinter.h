/******************************************************************************
 JPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPrinter
#define _H_JPrinter

#include "JBroadcaster.h"

class JPrinter : virtual public JBroadcaster
{
public:

	JPrinter();

	~JPrinter() override;

private:

	// not allowed

	JPrinter(const JPrinter&) = delete;
	JPrinter& operator=(const JPrinter&) = delete;
};

#endif
