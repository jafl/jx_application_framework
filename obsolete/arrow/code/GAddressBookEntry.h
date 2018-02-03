/******************************************************************************
 GAddressBookEntry.h

	Defines portable point type

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GAddressBookEntry
#define _H_GAddressBookEntry

#include <JString.h>

struct GAddressBookEntry
{
public:

	JString		fullname;
	JString		address;
	JString		fcc;
	JString		comment;
};
#endif
