/******************************************************************************
 GAddressBookEntry.h

	Defines portable point type

	Copyright © 1996 by John Lindal. All rights reserved.

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
